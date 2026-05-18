#pragma once

#include "assert.h"
#include "catchable.h"
#include "common/optional.h"
#include "error.h"
#include <list>
#include <mutex>

namespace xhey::camera {

#define FILTER_WORKSHOP(frame_type, frame_var)                                                                                   \
  std::make_shared<FilterWorkshop<std::shared_ptr< frame_type > > >([ = ](const std::shared_ptr< frame_type > & frame_var) -> bool

#define MAP_WORKSHOP2(frame_type1, frame_type2, frame_var)                                                                       \
  std::make_shared<MapWorkshop<std::shared_ptr< frame_type1 >, std::shared_ptr< frame_type2 > > >( \
      [ = ](std::shared_ptr< frame_type1 > frame_var) -> std::shared_ptr< frame_type2 >

#define MAP_WORKSHOP(frame_type, frame_var) MAP_WORKSHOP2(frame_type, frame_type, frame_var)

#define FLATMAP_WORKSHOP2(frame_type1, frame_type2, frame_var)                                                                   \
  std::make_shared<FlatmapWorkshop<std::shared_ptr< frame_type1 >, std::shared_ptr< frame_type2 > > >( \
      [ = ](std::shared_ptr< frame_type1 > frame_var) -> std::vector<std::shared_ptr< frame_type2 > >

#define FLATMAP_WORKSHOP(frame_type, frame_var) FLATMAP_WORKSHOP2(frame_type, frame_type, frame_var)

class Connectable : public Catchable<Error, Connectable> {
public:
    virtual ~Connectable() {};

    virtual Connectable *ConnectTo(const std::shared_ptr<Connectable> &connectable) {
        assert(false);
        return connectable.get();
    };

    virtual std::shared_ptr<Connectable> DisconnectFrom(const std::shared_ptr<Connectable> &connectable) {
        assert(false);
        return connectable;
    };

    virtual void DisconnectAll() { assert(false); }

    static uint32_t GenerateId() {
        static std::atomic<uint32_t> id = {1};
        return id++;
    }

    void SetName(const std::string &name) { name_ = name; }

    const std::string name() const { return name_; }

    uint32_t id() { return id_; }

protected:
    Connectable() : id_(GenerateId()) {};

    uint32_t id_;

private:
    std::string name_ = "Unknown";
};

template <class MediaType>
class MediaSinkInterface : virtual public Connectable {
public:
    MediaSinkInterface() { SetName("UnknownMediaSink"); }

    virtual ~MediaSinkInterface() {}

    virtual void OnMediaArrived(const MediaType &, uint32_t source_id) = 0;
};

template <class MediaType>
class MediaSourceInterface : virtual public Connectable {
public:
    MediaSourceInterface() { SetName("UnknownMediaSource"); }

    virtual ~MediaSourceInterface() {}

    virtual Connectable *ConnectTo(const std::shared_ptr<Connectable> &connectable) override {
        std::lock_guard<std::mutex> lg(mutex_);

        if (!connectable) {
            return this;
        }

        auto sink = std::dynamic_pointer_cast<MediaSinkInterface<MediaType>>(connectable);
        assert(sink != nullptr);
        auto iter = std::find(media_sinks_.begin(), media_sinks_.end(), sink);

        if (iter == media_sinks_.end()) {
            media_sinks_.push_back(sink);
        }

        return connectable.get();
    }

    virtual std::shared_ptr<Connectable> DisconnectFrom(const std::shared_ptr<Connectable> &connectable) override {
        std::lock_guard<std::mutex> lg(mutex_);
        auto sink = std::dynamic_pointer_cast<MediaSinkInterface<MediaType>>(connectable);

        if (sink == nullptr) {
            assert(false);
            printf("The connectable you connected must be base of MediaSinkInterface<MediaType>, Check your workshops.\n");
        }

        auto iter = std::find(media_sinks_.begin(), media_sinks_.end(), sink);

        if (iter == media_sinks_.end()) {
            return nullptr;
        }

        media_sinks_.erase(iter);
        return connectable;
    }

    virtual void DisconnectAll() override {
        std::lock_guard<std::mutex> lg(mutex_);
        media_sinks_.clear();
    }

    virtual bool Throw(const Error &error) override {
        bool handled = Catchable::Throw(error);
        if (handled) {
            return true;
        }
        std::lock_guard<std::mutex> lg(mutex_);
        for (auto &sink : media_sinks_) {
            handled = sink->Throw(error);
            if (handled) {
                break;
            }
        }
        return handled;
    }

protected:
    virtual void Publish(const MediaType &media_data) {
        std::lock_guard<std::mutex> lg(mutex_);

        for (auto sink : media_sinks_) {
            sink->OnMediaArrived(media_data, this->id_);
        }
    }

    virtual std::shared_ptr<MediaSinkInterface<MediaType>>
    FindMediaSink(const std::function<bool(std::shared_ptr<MediaSinkInterface<MediaType>>)> &condition) {
        std::lock_guard<std::mutex> lg(mutex_);
        auto iter = std::find_if(media_sinks_.begin(), media_sinks_.end(), condition);
        return iter == media_sinks_.end() ? nullptr : *iter;
    }

private:
    std::mutex mutex_;
    std::list<std::shared_ptr<MediaSinkInterface<MediaType>>> media_sinks_;
};

template <class MediaTypeA, class MediaTypeB = MediaTypeA>
class MediaWorkshop : public MediaSinkInterface<MediaTypeA>, public MediaSourceInterface<MediaTypeB> {
public:
    MediaWorkshop() { Connectable::SetName("UnknownWorkshop"); }

    virtual ~MediaWorkshop() {}
};

template <class MediaTypeIn, class MediaTypeOut = MediaTypeIn>
class MapWorkshop : public MediaWorkshop<MediaTypeIn, MediaTypeOut> {
public:
    typedef std::function<MediaTypeOut(const MediaTypeIn &media_data)> map_func_type;
    MapWorkshop(const map_func_type &map_func, const std::string &name = "") : map_func_(map_func) {
        assert(map_func != nullptr);
        Connectable::SetName(name.empty() ? "MapWorkshop" : name);
    };
    virtual ~MapWorkshop() {};

    virtual void OnMediaArrived(const MediaTypeIn &media_data, uint32_t source_id) override {
        MediaTypeOut ret = map_func_(media_data);
        this->Publish(ret);
    }

private:
    map_func_type map_func_;
};

template <class MediaTypeIn>
class FilterWorkshop : public MediaWorkshop<MediaTypeIn, MediaTypeIn> {
public:
    typedef std::function<bool(const MediaTypeIn &media_data)> filter_func_type;
    FilterWorkshop(const filter_func_type &filter_func, const std::string &name = "") : filter_func_(filter_func) {
        assert(filter_func_ != nullptr);
        Connectable::SetName(name.empty() ? "FilterWorkshop" : name);
    };
    virtual ~FilterWorkshop() {};

protected:
    virtual void OnMediaArrived(const MediaTypeIn &media_data, uint32_t source_id) override {
        bool ret = filter_func_(media_data);

        if (!ret) {
            this->Publish(media_data);
        }
    }
    filter_func_type filter_func_;
};

template <class MediaTypeIn, class MediaTypeOut = MediaTypeIn>
class FlatmapWorkshop : public MediaWorkshop<MediaTypeIn, MediaTypeOut> {
public:
    typedef std::function<std::vector<MediaTypeOut>(const MediaTypeIn &media_data)> map_func_type;
    FlatmapWorkshop(const map_func_type &map_func, const std::string &name = "") : map_func_(map_func) {
        assert(map_func_ != nullptr);
        Connectable::SetName(name.empty() ? "FlatmapWorkshop" : name);
    };
    virtual ~FlatmapWorkshop() {};

    virtual void OnMediaArrived(const MediaTypeIn &media_data, uint32_t source_id) override {
        std::vector<MediaTypeOut> ret = map_func_(media_data);

        for (auto i : ret) {
            this->Publish(i);
        }
    }

private:
    map_func_type map_func_;
};

template <class MediaType>
class MediaBank : public MediaSinkInterface<MediaType> {
public:
    MediaBank() { Connectable::SetName("UnknownMediaBank"); }

    MediaType FetchMediaData() {
        MediaType ret = media_.Value();
        media_.Reset();
        return ret;
    }

protected:
    virtual void OnMediaArrived(const MediaType &media, uint32_t source_id) override { media_.Reset(media); }

private:
    Optional<MediaType> media_;
};

template <class MediaType>
class MediaPublisher : public MediaSourceInterface<MediaType> {
public:
    MediaPublisher() { Connectable::SetName("MediaPublisher"); }

    void PublishMediaData(const MediaType &data) { this->Publish(data); }
};

} // namespace xhey::camera
