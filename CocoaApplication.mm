#import <AppKit/NSApplication.h>
#import "AppDelegate.h"
#import "WindowDelegate.h"
#import "CustomizedView.h"

static const CGFloat InitX = 960 + 20;
static const CGFloat InitY = 0;
static const CGFloat Width = 960;
static const CGFloat Height = 540;

int main(int argc, const char *argv[]) {
    int result = 0;

    [NSApplication sharedApplication];

    // Menu
    NSString *appName = [NSString stringWithFormat:@"%s", "Minimal Cocoa App"];
    id menubar = [[NSMenu alloc] initWithTitle:appName];
    id appMenuItem = [NSMenuItem new];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];

    id appMenu = [NSMenu new];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit"
                                                 action:@selector(terminate:)
                                          keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];

    id appDelegate = [AppDelegate new];
    [NSApp setDelegate:appDelegate];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp finishLaunching];

    NSInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                      NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    NSWindow *m_pWindow = [[NSWindow alloc]
            initWithContentRect:CGRectMake(InitX, InitY, Width,
                                           Height) styleMask:style backing:NSBackingStoreBuffered defer:NO];
    [m_pWindow setTitle:appName];
    [m_pWindow makeKeyAndOrderFront:nil];
    id winDelegate = [WindowDelegate new];
    [m_pWindow setDelegate:winDelegate];

    NSOpenGLPixelFormatAttribute attrs[] = {
            NSOpenGLPFAAccelerated,
            // OpenGL version matters
            // Legacy version is 2.1, some modern API maybe be unavailable.
            // In the meantime, some old & simple demo api like glBegin glColor3f is unavailable in high opengl version.
            // You might get `1282` error when you try to run old API in modern version of OpenGL.
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
//            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAColorSize, 32,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAStencilSize, 8,
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAMultisample,
            NSOpenGLPFASampleBuffers, 1,
            NSOpenGLPFASamples, 4, // 4x MSAA
            0
    };

    NSLog(@"Create CustomizedView");
    CustomizedView *pGLView = [CustomizedView new];
    NSLog(@"Init NSOpenGLPixelFormat");
    pGLView.pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if ([pGLView pixelFormat] == nil) {
        NSLog(@"No valid matching OpenGL Pixel Format found");
        [pGLView release];
        return -1;
    }

    NSLog(@"Init pGLView initWithFrame");
    [pGLView initWithFrame:CGRectMake(0, 0, Width, Height)];

    NSLog(@"Set window content view ad pGLView");
    [m_pWindow setContentView:pGLView];

    NSLog(@"Main Loop start");
    while (true) {
        NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:nil
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];

        switch ([(NSEvent *) event type]) {
            case NSEventTypeKeyDown:
                NSLog(@"Key Down Event Received!");
                break;
            default:
                break;
        }
        [NSApp sendEvent:event];
        [NSApp updateWindows];
        [event release];
        // update view
        [[m_pWindow contentView] setNeedsDisplay:YES];
    }

    return result;
}