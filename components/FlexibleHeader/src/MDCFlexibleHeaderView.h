// Copyright 2015-present the Material Components for iOS authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import <UIKit/UIKit.h>

#import "MaterialElevation.h"
// TODO(b/151929968): Delete import of delegate headers when client code has been migrated to no
// longer import delegates as transitive dependencies.
#import "MDCFlexibleHeaderViewAnimationDelegate.h"
#import "MDCFlexibleHeaderViewDelegate.h"
#import "MaterialShadowElevations.h"

FOUNDATION_EXPORT NSString *_Nonnull const MDCFlexibleHeaderViewAccessibilityIdentifier;

typedef void (^MDCFlexibleHeaderChangeContentInsetsBlock)(void);
typedef void (^MDCFlexibleHeaderShadowIntensityChangeBlock)(__kindof CALayer *_Nonnull shadowLayer,
                                                            CGFloat intensity);

/** Mutually exclusive phases that the flexible header view can be in. */
typedef NS_ENUM(NSInteger, MDCFlexibleHeaderScrollPhase) {

  /**
   The header is at its min height and shifting off/on screen.

   frame.origin.y is changing.
   */
  MDCFlexibleHeaderScrollPhaseShifting,

  /**
   The header is changing its height within the min-max range.

   frame.size.height is changing.
   */
  MDCFlexibleHeaderScrollPhaseCollapsing,

  /**
   The header is changing its height and is taller than its maximum height.

   frame.size.height is changing.
   */
  MDCFlexibleHeaderScrollPhaseOverExtending,
};

@protocol MDCFlexibleHeaderViewAnimationDelegate;
@protocol MDCFlexibleHeaderViewDelegate;

// TODO(b/238930139): Remove usage of this deprecated API.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
/**
 MDCFlexibleHeaderView tracks the content offset of a scroll view and adjusts its size and
 position according to a configurable set of behaviors.

 ### UIScrollViewDelegate forwarding

 This view relies on you informing it of certain UIScrollViewDelegate events as they happen. These
 events are listed in the UIScrollViewDelegate events section.
 */
IB_DESIGNABLE
@interface MDCFlexibleHeaderView : UIView <MDCElevatable, MDCElevationOverriding>
#pragma clang diagnostic pop

#pragma mark Custom shadow

/**
 Custom shadow shown under flexible header content.
 */
@property(nonatomic, strong, nullable) __kindof CALayer *shadowLayer;

/** The shadow color of the @c shadowLayer. Defaults to black. */
@property(nonatomic, copy, nonnull) UIColor *shadowColor;

/**
 Sets a custom shadow layer and a block that should be executed when shadow intensity changes.
 */
- (void)setShadowLayer:(nonnull __kindof CALayer *)shadowLayer
    intensityDidChangeBlock:(nonnull MDCFlexibleHeaderShadowIntensityChangeBlock)block;

#pragma mark UIScrollViewDelegate events

/**
 Informs the receiver that the tracking scroll view's contentOffset has changed.

 Must be called from the trackingScrollView delegate's UIScrollViewDelegate::scrollViewDidScroll:
 implementor.

 @note Do not invoke this method if self.observesTrackingScrollViewScrollEvents is YES.
 */
- (void)trackingScrollViewDidScroll;

/**
 Informs the receiver that the tracking scroll view's adjustedContentInset has changed.

 Must be called from the trackingScrollView delegate's
 UIScrollViewDelegate::scrollViewDidChangeAdjustedContentInset: implementor.

 @note Do not invoke this method if self.observesTrackingScrollViewScrollEvents is YES.
 */
- (void)trackingScrollViewDidChangeAdjustedContentInset:(nullable UIScrollView *)trackingScrollView
    API_AVAILABLE(ios(11.0), tvos(11.0));

#pragma mark Changing the tracking scroll view

/**
 Informs the receiver that the tracking scroll view might be about to change to a new tracking
 scroll view.
 */
- (void)trackingScrollWillChangeToScrollView:(nullable UIScrollView *)scrollView;

#pragma mark UIKit Hooks

// All of these UIKit hooks must be called from the view controller that owns this header view.
// Failure to do so will result in undefined behavior of the flexible header view.

/**
 Returns a Boolean value indicating whether the status bar should be visible.

 Must be called by the owning UIViewController's -prefersStatusBarHidden.
 */
@property(nonatomic, readonly) BOOL prefersStatusBarHidden;

/**
 Informs the receiver that the owning view controller's size will change.

 Must be called from UIViewController::viewWillTransitionToSize:withTransitionCoordinator: on apps
 targeting iOS 8 and onward.
 */
- (void)viewWillTransitionToSize:(CGSize)size
       withTransitionCoordinator:(nonnull id<UIViewControllerTransitionCoordinator>)coordinator;

#pragma mark Changing Content Insets

/**
 Must be called by a client that wishes to update the content insets of the tracking scroll view.

 Not using this method can lead to undefined behavior due to the flexible header view assuming
 that a certain amount of additional content insets have been provided to the tracking scroll
 view.

 The provided block will be executed after the flexible header has removed its modifications to
 the tracking scroll view. Upon completion of the block, the flexible header will re-inject these
 modifications into the new content insets and ensure that the content offset doesn't change due
 to the new content insets.
 */
- (void)changeContentInsets:(nonnull MDCFlexibleHeaderChangeContentInsetsBlock)block;

#pragma mark - Animating changes to the header

/**
 Animates any changes resulting from executing the @c animations block.

 Use this method to animate changes alongside animations to the flexible header.

 The following occurs when this method is invoked:

 1. The provided @c animations block is invoked within a @code [UIView animate...:] @endcode block.
 2. Within that same animation block and after invoking @c animations, the flexible header updates
    any relevant aspects of its layout including its height, offset, and shadow layer frames.
 3. Upon completion of the resulting animation, the provided completion block is invoked if one was
    provided.
 */
- (void)animateWithAnimations:(void (^_Nonnull)(void))animations
                   completion:(void (^_Nullable)(BOOL))completion;

#pragma mark Forwarding Touch Events

/**
 Forwards any tap events made to the provided view on to the tracking scroll view.

 Views will only forward their taps if they are a subview of this header view and are interactive.

 Touch forwarding does not apply to subviews of the provided view.
 */
- (void)forwardTouchEventsForView:(nonnull UIView *)view;

/** Stops forwarding tap events on the given view to the tracking scroll view. */
- (void)stopForwardingTouchEventsForView:(nonnull UIView *)view;

#pragma mark Scroll Phase

/**
 Returns the current scroll phase of the flexible header.

 There are three mutually-exclusive scroll phases: shifting, collapsing, and over-extending.
 Whichever phase the header view is in governs what scrollPhaseValue and scrollPhasePercentage
 represent.

 This and the related scrollPhase properties are only valid immediately after a call to
 -trackingScrollViewDidScroll.
 */
@property(nonatomic, readonly) MDCFlexibleHeaderScrollPhase scrollPhase;

/**
 A value in screen points denoting the absolute position within the current scroll phase.

 The range for each phase follows:

 - Shifting:       [0, minimumHeight)
 - Collapsing:     [minimumHeight, maximumHeight)
 - Over-extending: [maximumHeight, +inf)
 */
@property(nonatomic, readonly) CGFloat scrollPhaseValue;

/**
 A normalized value denoting the position within the current scroll phase.

 The meaning of the percentage for each phase follows:

 - Shifting:       0 is unshifted, 1.0 is fully shifted off-screen
 - Collapsing:     0 == minimumHeight, 1.0 == maximumHeight
 - Over-extending: 1.0 height == maximumHeight, every additional 1.0 is one maximumHeight unit

 Note that a single percentage does not necessarily have equal weight between the three phases, so
 you should not use this value for any behavior that is active across any two phases; use
 scrollPhaseValue instead.
 */
@property(nonatomic, readonly) CGFloat scrollPhasePercentage;

#pragma mark Bounding Dimensions

/**
 The minimum height that this header can shrink to.

 See minMaxHeightIncludesSafeArea to learn how this number is used when the Safe Area changes.

 If you change the value of this property and the maximumHeight of the receiver is below the new
 minimumHeight, maximumHeight will be adjusted to match the new minimum value.
 */
@property(nonatomic) CGFloat minimumHeight;

/**
 The maximum height that this header can expand to.

 See minMaxHeightIncludesSafeArea to learn how this number is used when the Safe Area changes.

 If you change the value of this property and the minimumHeight of the receiver is above the new
 maximumHeight, minimumHeight will be adjusted to match the new maximumHeight.
 */
@property(nonatomic) CGFloat maximumHeight;

/**
 A layout guide that equates to the top safe area inset of the flexible header view.

 Use this layout guide to position subviews in the flexible header in relation to the top safe area
 insets.

 This object is intended to be used as a constraint item.
 */
@property(nonatomic, nonnull, readonly) UIView *topSafeAreaGuide;

#pragma mark Behaviors

/**
 Whether or not the header view is allowed to expand past its maximum height when the tracking
 scroll view has been dragged past its top edge.

 Default: YES
 */
@property(nonatomic) BOOL canOverExtend;

@property(nonatomic) float visibleShadowOpacity;  ///< The visible shadow opacity. Default: 0.4

#pragma mark Scroll View Tracking

/**
 The scroll view whose content offset affects the height/offset of the flexible header view.

 The receiver will inject the maximum height of the header view into the top component of the
 tracking scroll view's content insets. This ensures that there is enough space in the top insets
 to fit the header. This should be taken into account when working with the tracking scroll view's
 content insets.

 Importantly, if you wish to make changes to the tracking scroll view's content insets after it
 has been registered to a flexible header view, you must do so from within a -changeContentInsets:
 invocation on the flexible header view.

 The tracking scroll view is weakly held so that we don't unintentionally keep the scroll view
 around any longer than it needs to be. Doing so could get into tricky situations where the view
 controller didn't nil out the scroll view's delegate in dealloc and UIScrollView's non-weak
 delegate points to a dead object.
 */
@property(nonatomic, weak, nullable) UIScrollView *trackingScrollView;

/**
 Whether to automatically observe the trackingScrollView's content offset changes.

 When enabled, the header view will observe the contentOffset property of the tracking scroll view
 and react to changes accordingly.

 You must not forward any scroll view events to the header view if this property is enabled. Any
 attempts to do so will result in an assertion.

 If you attempt to enable this property when shiftBehavior is set to anything other than
 MDCFlexibleHeaderShiftBehaviorDisabled, an assertion will be thrown. If you intend to use any
 shifting behavior you must manually forward the necessary scroll view events.

 @note If you enable this property and you support iOS 10.3 or below, you are responsible for
 explicitly nilling out the tracking scroll view before it is deallocated. This is not required if
 your minimum OS is iOS 11 or above. Failure to nil out the tracking scroll view may lead to runtime
 crashes due to dangling observers on the tracking scroll view. Most commonly, the tracking scroll
 view can be nil'd out in the view controller's dealloc method. An example of the error you might
 see is: "An instance of class UITableView was deallocated while key value observers were still
 registered with it."

 Default: NO
 */
@property(nonatomic) BOOL observesTrackingScrollViewScrollEvents;

/**
 Whether or not the header is floating in front of an infinite stream of content.

 Enabling this behavior will cause the header to always appear to be floating "in front of" the
 content in Material space. This behavior should _only_ be enabled for content that has no top
 edge, e.g. an infinite stream of vertical content.

 Default: NO
 */
@property(nonatomic, getter=isInFrontOfInfiniteContent) BOOL inFrontOfInfiniteContent;

/**
 Whether or not the receiver is shared by many scroll views, such as in a tabbed interface with
 many columns of content.

 Default: NO
 */
@property(nonatomic) BOOL sharedWithManyScrollViews;

/**
 If enabled, the trackingScrollView doesn't adjust the content inset when its
 contentInsetAdjustmentBehavior is set to be UIScrollViewContentInsetAdjustmentNever.

 Default: NO
 */
@property(nonatomic)
    BOOL disableContentInsetAdjustmentWhenContentInsetAdjustmentBehaviorIsNever API_AVAILABLE(
        ios(11.0), tvos(11.0));

#pragma mark Delegation

/** The delegate for this header view. */
@property(nonatomic, weak, nullable) id<MDCFlexibleHeaderViewDelegate> delegate;

/** The animation delegate will be notified of any animations to the flexible header view. */
@property(nonatomic, weak, nullable) id<MDCFlexibleHeaderViewAnimationDelegate> animationDelegate;

/**
 A block that is invoked when the FlexibleHeaderView receives a call to @c
 traitCollectionDidChange:. The block is called after the call to the superclass.
 */
@property(nonatomic, copy, nullable) void (^traitCollectionDidChangeBlock)
    (MDCFlexibleHeaderView *_Nonnull flexibleHeaderView,
     UITraitCollection *_Nullable previousTraitCollection);

/**
 The elevation of the header.
 */
@property(nonatomic, assign) MDCShadowElevation elevation;

@end

@interface MDCFlexibleHeaderView (ToBeDeprecated)


/**
 When this is enabled, the flexible header will assume that minimumHeight and maximumHeight both
 include the Safe Area top inset. For example, a header whose maximum content height should be 200
 might set 220 (200 + 20) as the maximumHeight. Notice that if this is enabled and you're setting
 minimumHeight and or maximumHeight, the flexible header won't automatically adjust its size to
 account for changes to the Safe Area, as the values provided already include a hardcoded inset.

 When this is disabled, the flexible header will assume that the provided minimumHeight and
 maximumHeight do not include the Safe Area top inset. For example, a header whose maximum content
 height should be 200 would set 200 as the maximumHeight, and the flexible header will take care
 of adjusting itself to account for Safe Area changes internally.

 Clients are recommended to set this to NO, and set the min and max heights to values that don't
 include the status bar or Safe Area insets.

 @warning This API will soon be disabled by default and then deprecated. Learn more at
 https://github.com/material-components/material-components-ios/blob/develop/components/FlexibleHeader/docs/migration-guide-minMaxHeightIncludesSafeArea.md

 Default is YES.
 */
@property(nonatomic) BOOL minMaxHeightIncludesSafeArea;

/**
 * Whether or not shadow opacity (if using the default shadow layer) should be reset to 0 when
 * trackingScrollView is set to nil. If the flexible header view is created without ever setting
 * trackingScrollView, it always has 0 opacity for the default shadow layer regardless of the value
 * of this flag. If trackingScrollView is ever set, then this flag enables resetting the shadow
 * opacity back to 0 when trackingScrollView is set to nil.
 *
 * Default: NO, but we are planning to change it to YES very soon, so all clients should set this
 * property to YES.
 */
@property(nonatomic) BOOL resetShadowAfterTrackingScrollViewIsReset;

/**
 Whether to allow shadow frame animations when animating changes to the tracking scroll view.

 Enabling this property allows layoutSubviews to animate the shadow frames as part of the animation
 that occurs when changing tracking scroll views.

 This property will eventually be enabled by default and then deleted.

 Default is NO.
 */
@property(nonatomic, assign) BOOL allowShadowLayerFrameAnimationsWhenChangingTrackingScrollView;

@end

@interface MDCFlexibleHeaderView (Deprecated)

// Pre-iOS 8 Interface Orientation APIs

/**
 Informs the receiver that the interface orientation is about to change.

 Must be called from UIViewController::willRotateToInterfaceOrientation:duration:.
 */
- (void)interfaceOrientationWillChange __deprecated_msg(
    "Use viewWillTransitionToSize:withTransitionCoordinator: instead.");

/**
 Informs the receiver that the interface orientation is in the process of changing.

 Must be called from UIViewController::willAnimateRotationToInterfaceOrientation:duration:.
 */
- (void)interfaceOrientationIsChanging __deprecated_msg(
    "Use viewWillTransitionToSize:withTransitionCoordinator: instead.");

/**
 Informs the receiver that the interface orientation has changed.

 Must be called from UIViewController::didRotateFromInterfaceOrientation:.
 */
- (void)interfaceOrientationDidChange __deprecated_msg(
    "Use viewWillTransitionToSize:withTransitionCoordinator: instead.");

@end

// clang-format off
@interface MDCFlexibleHeaderView ()

#pragma mark Accessing the header's views

/** Deprecated. Please register views directly to the flexible header. */
@property(nonatomic, strong, nonnull) UIView *contentView
__deprecated_msg("Please register views directly to the flexible header.");

@end
// clang-format on
