#import "CustomizedView.h"
#import "GraphicsManager.h"

namespace Gm {
    Gm::GraphicsManager *g_pGraphicsManager = reinterpret_cast<Gm::GraphicsManager *>(new Gm::GraphicsManager);
}

@implementation CustomizedView

- (void)drawRect:(NSRect)dirtyRect {
    // TODO: lockFocus
    if (_openGLContext.view != self) {
        [_openGLContext setView:self];
        Gm::g_pGraphicsManager->Initialize();
    }
    [_openGLContext makeCurrentContext];

    Gm::g_pGraphicsManager->Clear();
    Gm::g_pGraphicsManager->Draw();

    [_openGLContext flushBuffer];
}

- (void)mouseDown:(NSEvent *)event {
    [self drawRect:[self frame]];
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    // TODO this method had been called twice?
    self = [super initWithFrame:frameRect];
    // create NSOpenGLContext
    NSLog(@"Create NSOpenGLContext");
    _openGLContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:nil];
    [_openGLContext makeCurrentContext];
    if (self != nil) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_surfaceNeedsUpdate:)
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
    }
    return self;
}

- (void)_surfaceNeedsUpdate:(NSNotification *)notification {
    [self update];
}

- (void)lockFocus {
    NSOpenGLContext *context = _openGLContext;

    [super lockFocus];
    if ([context view] != self) {
        [context setView:self];
    }
    [context makeCurrentContext];
}

- (void)update {
    [_openGLContext update];
}

- (void)dealloc {
    [_pixelFormat release];
    [_openGLContext release];
    Gm::g_pGraphicsManager->Finalize();
    [super dealloc];
}

- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];
    if ([self window] == nil)
        [_openGLContext clearDrawable];
}

@end

