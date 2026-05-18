//
//  XHCameraLocationManager.h
//  XheyCameraSDK
//
//  Created by leizh007 on 2025/3/24.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

@class XHCameraLocationManager;

@protocol XHCameraLocationManagerDelegate <NSObject>

- (void)locationManager:(XHCameraLocationManager *)manager
      didUpdateLocation:(CLLocation *)location;

- (void)locationManager:(XHCameraLocationManager *)manager
       didUpdateAzimuth:(double)azimuth;
@end

@interface XHCameraLocationManager : NSObject

@property (nonatomic, weak) id<XHCameraLocationManagerDelegate> delegate;

@property (nonatomic, weak) UIViewController *viewController;

@property (nonatomic, strong) CLLocation *location;

@property (nonatomic, assign) double azimuth;

- (BOOL)getAuthorizationStatus;

- (void)startUpdatingLocation;

- (void)stopUpdatingLocation;

@end

NS_ASSUME_NONNULL_END
