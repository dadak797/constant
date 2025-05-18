#import <Cocoa/Cocoa.h>
#import "mac_helper.h"


float MacHelper::DevicePixelRatio() {
    return [[NSScreen mainScreen] backingScaleFactor];
}