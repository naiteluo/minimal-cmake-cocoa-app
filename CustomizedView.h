#import <Cocoa/Cocoa.h>

@interface CustomizedView : NSView {
@private
    NSOpenGLContext *_openGLContext;

    GLuint program;
    GLuint vbo, vao;
}

@property(nonatomic, strong) NSOpenGLPixelFormat *pixelFormat;

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format;

- (void)mouseDown:(NSEvent *)event;

- (void)mouseDragged:(NSEvent *)event;

- (void)scrollWheel:(NSEvent *)event;

- (void)update;

- (void)dealloc;

- (void)viewDidMoveToWindow;

- (void)setPixelFormat:(NSOpenGLPixelFormat *)pixelFormat;

- (NSOpenGLPixelFormat *)pixelFormat;

@end
