#import <AppKit/NSApplication.h>
#import "AppDelegate.h"
#import "WindowDelegate.h"
#import "CustomizedView.h"

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
            initWithContentRect:CGRectMake(0, 0, 600,
                                           600) styleMask:style backing:NSBackingStoreBuffered defer:NO];
    [m_pWindow setTitle:appName];
    [m_pWindow makeKeyAndOrderFront:nil];
    id winDelegate = [WindowDelegate new];
    [m_pWindow setDelegate:winDelegate];

    NSOpenGLPixelFormatAttribute attrs[] = {
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
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

    NSLog(@"Before GLView *pGLView = [GLView new];");
    CustomizedView *pGLView = [CustomizedView new];
    NSLog(@"After GLView *pGLView = [GLView new];");
    pGLView.pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    NSLog(@"CocoaOpenGLApplication::Initialize");
    if ([pGLView pixelFormat] == nil) {
        NSLog(@"No valid matching OpenGL Pixel Format found");
        [pGLView release];
        return -1;
    }

    NSLog(@"pGLView initWithFrame:CGRectMake");
    [pGLView initWithFrame:CGRectMake(0, 0, 600, 600)];

    [m_pWindow setContentView:pGLView];

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
    }

    return result;
}