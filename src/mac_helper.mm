#import "mac_helper.h"

// MacOS api headers
#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>


float MacHelper::DevicePixelRatio() {
    return [[NSScreen mainScreen] backingScaleFactor];
}

std::vector<std::string> MacHelper::OpenFileDialog() {
    std::vector<std::string> selectedFiles;

    @autoreleasepool {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:YES];
        if (@available(macOS 12.0, *)) {
            NSMutableArray *contentTypes = [NSMutableArray array];
            [contentTypes addObject:[UTType typeWithFilenameExtension:@"jpg"]];
            [contentTypes addObject:[UTType typeWithFilenameExtension:@"png"]];
            [contentTypes addObject:[UTType typeWithFilenameExtension:@"txt"]];
            [contentTypes addObject:[UTType typeWithFilenameExtension:@"pdf"]];
            [panel setAllowedContentTypes:contentTypes];
        }
        else {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdeprecated-declarations"
            [panel setAllowedFileTypes:@[@"jpg", @"png", @"txt", @"pdf"]];
            #pragma clang diagnostic pop
        }

        if ([panel runModal] == NSModalResponseOK) {
            NSArray<NSURL *> *urls = [panel URLs];
            for (NSURL *url in urls) {
                selectedFiles.push_back(std::string([[url path] UTF8String]));
            }
        }
    }

    return selectedFiles;
}