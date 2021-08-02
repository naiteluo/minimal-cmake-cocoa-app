#import <Cocoa/Cocoa.h>

@interface CustomizedView : NSView {
    @private
    NSOpenGLContext *_openGLContext;
}

@property(nonatomic, strong) NSOpenGLPixelFormat *pixelFormat;

@end
