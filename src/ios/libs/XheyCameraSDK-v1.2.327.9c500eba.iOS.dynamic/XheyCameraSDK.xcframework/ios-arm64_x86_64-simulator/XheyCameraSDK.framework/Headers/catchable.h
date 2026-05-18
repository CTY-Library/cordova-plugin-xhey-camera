#pragma once

#include <functional>
#include <memory>

namespace xhey::camera {

template <typename ErrorObject>
class ErrorHandler {
public:
    virtual ~ErrorHandler() {}
    virtual bool OnError(const ErrorObject &error) = 0;
};


template <typename ErrorObject>
class LambdaErrorHandler : public ErrorHandler<ErrorObject> {
public:
    LambdaErrorHandler(const std::function<bool(const ErrorObject &)> &handler) : handler_(handler) {}
    virtual ~LambdaErrorHandler() {}
    virtual bool OnError(const ErrorObject &error) override { return handler_ ? handler_(error) : false; }

private:
    std::function<bool(const ErrorObject &)> handler_;
};


template <typename ErrorObject, class ChildType>
class Catchable {
public:
    virtual ~Catchable() {}

    virtual bool Throw(const ErrorObject &error) {
        bool handled = false;
        if (handler_) {
            handled = handler_->OnError(error);
        }
        return handled;
    }

    ChildType *Catch(const std::shared_ptr<ErrorHandler<ErrorObject>> &handler) {
        handler_ = handler;
        return (ChildType *)this;
    }

    ChildType *Catch(const std::function<bool(const ErrorObject &)> &lambda) {
        handler_ = std::make_shared<LambdaErrorHandler<ErrorObject>>(lambda);
        return (ChildType *)this;
    }

private:
    std::shared_ptr<ErrorHandler<ErrorObject>> handler_;
};

} // namespace xhey::camera
