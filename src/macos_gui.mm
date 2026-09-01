#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include "image_processing.hpp"
#include "png_writer.hpp"
#include "raster_renderer.hpp"
#include "score.hpp"
#include "svg_renderer.hpp"

#include <algorithm>
#include <fstream>
#include <string>

@interface NotationalRuinController : NSObject {
    NSWindow* window;
    NSImageView* preview;
    NSTextField* seedField;
    NSTextField* status;
    NSSlider* density;
    NSSlider* chaos;
    NSSlider* threshold;
    NSSlider* blur;
    NSSlider* downsample;
    NSSlider* xeroxDepth;
    NSButton* exportPngButton;
    NSButton* exportSvgButton;
    NSString* currentPngPath;
    NSString* currentSvgPath;
}
- (void)show;
- (void)generate:(id)sender;
- (void)exportFile:(id)sender;
@end

static NSTextField* label(NSString* title, NSRect frame) {
    NSTextField* field = [[NSTextField alloc] initWithFrame:frame];
    field.stringValue = title;
    field.editable = NO;
    field.bezeled = NO;
    field.drawsBackground = NO;
    return field;
}

static NSSlider* slider(NSRect frame, double min, double max, double value) {
    NSSlider* control = [[NSSlider alloc] initWithFrame:frame];
    control.minValue = min;
    control.maxValue = max;
    control.doubleValue = value;
    control.continuous = NO;
    return control;
}

@implementation NotationalRuinController

- (instancetype)init {
    self = [super init];
    if (self) {
        window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1080, 700)
                                              styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
                                                backing:NSBackingStoreBuffered defer:NO];
        window.title = @"Notational Ruin";
        [window center];

        NSView* content = window.contentView;
        [content addSubview:label(@"PROCEDURAL SCORE / REPRODUCTION MACHINE", NSMakeRect(28, 650, 430, 24))];
        [content addSubview:label(@"Seed", NSMakeRect(28, 602, 120, 22))];
        seedField = [[NSTextField alloc] initWithFrame:NSMakeRect(145, 598, 120, 28)];
        seedField.stringValue = @"18";
        [content addSubview:seedField];

        density = slider(NSMakeRect(145, 550, 220, 20), 0, 1, .7);
        chaos = slider(NSMakeRect(145, 500, 220, 20), 0, 1, .4);
        threshold = slider(NSMakeRect(145, 450, 220, 20), 0, 1, .52);
        blur = slider(NSMakeRect(145, 400, 220, 20), 0, 5, 1.8);
        downsample = slider(NSMakeRect(145, 350, 220, 20), .1, 1, 1);
        xeroxDepth = slider(NSMakeRect(145, 300, 220, 20), 0, 8, 3);
        NSArray* names = @[@"Density", @"Chaos", @"Threshold", @"Blur", @"Downsample", @"Xerox depth"];
        NSArray* controls = @[density, chaos, threshold, blur, downsample, xeroxDepth];
        for (NSInteger i = 0; i < names.count; ++i) {
            CGFloat y = 550 - i * 50;
            [content addSubview:label(names[i], NSMakeRect(28, y, 110, 22))];
            [content addSubview:controls[i]];
        }

        NSButton* button = [[NSButton alloc] initWithFrame:NSMakeRect(28, 220, 337, 38)];
        button.title = @"Generate Ruin";
        button.bezelStyle = NSBezelStyleRounded;
        button.target = self;
        button.action = @selector(generate:);
        [content addSubview:button];

        exportPngButton = [[NSButton alloc] initWithFrame:NSMakeRect(28, 170, 162, 34)];
        exportPngButton.title = @"Export PNG…";
        exportPngButton.bezelStyle = NSBezelStyleRounded;
        exportPngButton.target = self;
        exportPngButton.action = @selector(exportFile:);
        exportPngButton.tag = 1;
        exportPngButton.enabled = NO;
        [content addSubview:exportPngButton];

        exportSvgButton = [[NSButton alloc] initWithFrame:NSMakeRect(203, 170, 162, 34)];
        exportSvgButton.title = @"Export SVG…";
        exportSvgButton.bezelStyle = NSBezelStyleRounded;
        exportSvgButton.target = self;
        exportSvgButton.action = @selector(exportFile:);
        exportSvgButton.tag = 2;
        exportSvgButton.enabled = NO;
        [content addSubview:exportSvgButton];

        status = label(@"Ready", NSMakeRect(28, 128, 400, 22));
        [content addSubview:status];

        preview = [[NSImageView alloc] initWithFrame:NSMakeRect(400, 28, 650, 630)];
        preview.imageScaling = NSImageScaleProportionallyUpOrDown;
        preview.imageAlignment = NSImageAlignCenter;
        preview.wantsLayer = YES;
        preview.layer.backgroundColor = [NSColor colorWithWhite:.93 alpha:1].CGColor;
        [content addSubview:preview];
    }
    return self;
}

- (void)show { [window makeKeyAndOrderFront:nil]; [self generate:nil]; }

- (void)generate:(id)sender {
    (void)sender;
    int width = 1200, height = 800;
    ScoreParameters q;
    q.width = width;
    q.height = height;
    q.seed = static_cast<uint32_t>(std::stoul(seedField.stringValue.UTF8String));
    q.density = static_cast<float>(density.floatValue);
    q.chaos = static_cast<float>(chaos.floatValue);
    float cut = static_cast<float>(threshold.floatValue);
    float radius = static_cast<float>(blur.floatValue);
    float reduction = static_cast<float>(downsample.floatValue);
    int generations = static_cast<int>(xeroxDepth.intValue);

    Score score = generateScore(q);
    NSString* base = [NSTemporaryDirectory() stringByAppendingPathComponent:@"notational-ruin-gui"];
    std::string prefix = base.UTF8String;
    std::ofstream svg(prefix + "_clean.svg");
    svg << renderSvg(score);
    svg.close();

    Image image = renderRaster(score);
    int smallW = std::max(1, static_cast<int>(width * reduction));
    int smallH = std::max(1, static_cast<int>(height * reduction));
    image = resizeNearest(image, smallW, smallH);
    image = thresholdImage(addNoise(blurImage(image, radius), .15f, q.seed), cut);
    image = erodeImage(image, 1);
    for (int generation = 0; generation < generations; ++generation)
        image = xeroxGeneration(image, radius * .55f + generation * .12f, cut, .07f, q.seed + generation + 1);
    image = resizeNearest(image, width, height);
    std::string pngPath = prefix + "_ruined.png";
    if (writePng(image, pngPath)) {
        currentPngPath = [NSString stringWithUTF8String:pngPath.c_str()];
        currentSvgPath = [NSString stringWithUTF8String:(prefix + "_clean.svg").c_str()];
        preview.image = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:pngPath.c_str()]];
        exportPngButton.enabled = YES;
        exportSvgButton.enabled = YES;
        status.stringValue = [NSString stringWithFormat:@"Generated seed %u  •  %.0f%% resolution  •  %d copies", q.seed, reduction * 100, generations];
    } else {
        status.stringValue = @"Could not write preview";
    }
}

- (void)exportFile:(id)sender {
    if (!currentPngPath || !currentSvgPath) return;
    BOOL png = [sender tag] == 1;
    NSString* sourcePath = png ? currentPngPath : currentSvgPath;
    NSString* extension = png ? @"png" : @"svg";
    NSString* suggestedName = png ? @"notational-ruin.png" : @"notational-ruin-clean.svg";

    NSSavePanel* panel = [NSSavePanel savePanel];
    panel.nameFieldStringValue = suggestedName;
    panel.allowedContentTypes = @[png ? UTTypePNG : UTTypeSVG];
    panel.canCreateDirectories = YES;
    [panel beginSheetModalForWindow:window completionHandler:^(NSModalResponse response) {
        if (response != NSModalResponseOK) return;
        NSData* data = [NSData dataWithContentsOfFile:sourcePath];
        if ([data writeToURL:panel.URL atomically:YES]) {
            self->status.stringValue = [NSString stringWithFormat:@"Exported %@", panel.URL.lastPathComponent];
        } else {
            self->status.stringValue = @"Export failed";
        }
    }];
}
@end

int main() {
    @autoreleasepool {
        NSApplication* application = [NSApplication sharedApplication];
        application.activationPolicy = NSApplicationActivationPolicyRegular;
        NotationalRuinController* controller = [[NotationalRuinController alloc] init];
        [controller show];
        [application activateIgnoringOtherApps:YES];
        [application run];
    }
    return 0;
}
