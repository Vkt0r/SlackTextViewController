//
//   Copyright 2014 Slack Technologies, Inc.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#import <UIKit/UIKit.h>
#import "SLKTextInputbar.h"
#import "SLKTypingIndicatorView.h"
#import "SLKTextView.h"

#import "UIScrollView+SLKAdditions.h"
#import "UITextView+SLKAdditions.h"
#import "UIView+SLKAdditions.h"

/** UIKeyboard notification replacement, posting reliably only when showing/hiding the
 keyboard (not when resizing keyboard, or with inputAccessoryView reloads, etc.)
 Use these APIs with your own risk (still a work in progress). */
extern NSString *const SLKKeyboardWillShowNotification;
extern NSString *const SLKKeyboardDidShowNotification;
extern NSString *const SLKKeyboardWillHideNotification;
extern NSString *const SLKKeyboardDidHideNotification;

typedef NS_ENUM(NSUInteger, SLKKeyboardStatus) {
    SLKKeyboardStatusDidHide,
    SLKKeyboardStatusWillShow,
    SLKKeyboardStatusDidShow,
    SLKKeyboardStatusWillHide
};

/** @name A drop-in UIViewController subclass with a growing text input view and other useful messaging features. */
@interface SLKTextViewController : UIViewController <UITextViewDelegate, UITableViewDelegate, UITableViewDataSource, UICollectionViewDelegate, UICollectionViewDataSource>

/** The main table view managed by the controller object. Created by default initializing with -init or initWithNibName:bundle: */
@property (nonatomic, readonly) UITableView *tableView;

/** The main collection view managed by the controller object. Not nil if the controller is initialised with -initWithCollectionViewLayout: */
@property (nonatomic, readonly) UICollectionView *collectionView;

/** The bottom toolbar containing a text view and buttons. */
@property (nonatomic, readonly) SLKTextInputbar *textInputbar;

/** The typing indicator used to display user names horizontally. */
@property (nonatomic, readonly) SLKTypingIndicatorView *typingIndicatorView;

/** The single tap gesture used to dismiss the keyboard */
@property (nonatomic, readonly) UIGestureRecognizer *singleTapGesture;

/** YES if control's animation should have bouncy effects. Default is YES. */
@property (nonatomic, assign) BOOL bounces;

/** YES if text view's content can be cleaned with a shake gesture. Default is NO. */
@property (nonatomic, assign) BOOL undoShakingEnabled;

/** YES if keyboard can be dismissed gradually with a vertical panning gesture. Default is YES. */
@property (nonatomic, assign) BOOL keyboardPanningEnabled;

/** YES if an external keyboard has been detected (this value only changes when the text view becomes first responder). */
@property (nonatomic, readonly, getter=isExternalKeyboardDetected) BOOL externalKeyboardDetected;

/**
 YES if the main table view is inverted. Default is YES.
 @discussion This allows the table view to start from the bottom like any typical messaging interface.
 If inverted, you must assign the same transform property to your cells to match the orientation (ie: cell.transform = tableView.transform;)
 Inverting the table view will enable some great features such as content offset corrections automatically when resizing the text input and/or showing autocompletion.
 
 Updating this value also changes 'edgesForExtendedLayout' value. When inverted, it must be UIRectEdgeNone, to display correctly all the elements. Otherwise, UIRectEdgeAll is set.
 */
@property (nonatomic, assign, getter = isInverted) BOOL inverted;

/** YES if the view controller is presented inside of a popover controller. If YES, the keyboard won't move the text input bar and tapping on the tableView/collectionView will not cause the keyboard to be dismissed. This doesn't do anything on iPhone. */
@property (nonatomic, getter = isPresentedInPopover) BOOL presentedInPopover;

/** Convenience accessors (accessed through the text input bar) */
@property (nonatomic, readonly) SLKTextView *textView;
@property (nonatomic, readonly) UIButton *leftButton;
@property (nonatomic, readonly) UIButton *rightButton;


///------------------------------------------------
/// @name Initialization
///------------------------------------------------

/**
 Initializes a text view controller to manage a table view of a given style.
 @discussion If you use the standard -init method, a table view with plain style will be created.
 
 @param style A constant that specifies the style of main table view that the controller object is to manage (UITableViewStylePlain or UITableViewStyleGrouped).
 @return An initialized SLKTextViewController object or nil if the object could not be created.
 */
- (instancetype)initWithTableViewStyle:(UITableViewStyle)style;

/**
 Initializes a text view controller controller and configures the collection view with the provided layout.
 @discussion If you use the standard -init method, a table view with plain style will be created.

 @param layout The layout object to associate with the collection view. The layout controls how the collection view presents its cells and supplementary views.
 @return An initialized SLKTextViewController object or nil if the object could not be created.
 */
- (instancetype)initWithCollectionViewLayout:(UICollectionViewLayout *)layout;

/**
 Returns the tableView style to be configured when using Interface Builder. Default is UITableViewStylePlain.
 @discussion You must override this method if you want to configure a tableView.
 You should not override -initWithCoder:
 
 @param decoder An unarchiver object.
 @return The tableView style to be used in the new instantiated tableView.
 */
+ (UITableViewStyle)tableViewStyleForCoder:(NSCoder *)decoder;

/**
 Returns the tableView style to be configured when using Interface Builder. Default is nil.
 @discussion You must override this method if you want to configure a collectionView.
 You should not override -initWithCoder:
 
 @param decoder An unarchiver object.
 @return The collectionView style to be used in the new instantiated collectionView.
 */
+ (UICollectionViewLayout *)collectionViewLayoutForCoder:(NSCoder *)decoder;


///------------------------------------------------
/// @name Text Typing & Keyboard Handling
///------------------------------------------------

/**
 Presents the keyboard, if not already, animated.
 
 @param animated YES if the keyboard should show using an animation.
 */
- (void)presentKeyboard:(BOOL)animated;

/**
 Dimisses the keyboard, if not already, animated.
 
 @param animated YES if the keyboard should be dismissed using an animation.
 */
- (void)dismissKeyboard:(BOOL)animated;

/**
 Notifies the view controller that the keyboard changed status.
 @discussion You can override this method to perform additional tasks associated with presenting the view. You don't need call super since this method doesn't do anything.
 
 @param status The new keyboard status.
 */
- (void)didChangeKeyboardStatus:(SLKKeyboardStatus)status;


///------------------------------------------------
/// @name Text Typing Notifications
///------------------------------------------------

/**
 Notifies the view controller that the text will update.
 @discussion You can override this method to perform additional tasks associated with presenting the view. You MUST call super at some point in your implementation.
 */
- (void)textWillUpdate NS_REQUIRES_SUPER;

/**
 Notifies the view controller that the text did update.
 @discussion You can override this method to perform additional tasks associated with presenting the view. You MUST call super at some point in your implementation.
 
 @param If YES, the text input bar will be resized using an animation.
 */
- (void)textDidUpdate:(BOOL)animated NS_REQUIRES_SUPER;

/**
 Notifies the view controller when the left button's action has been triggered, manually.
 @discussion You can override this method to perform additional tasks associated with the left button. You don't need call super since this method doesn't do anything.
 
 @param sender The object calling this method.
 */
- (void)didPressLeftButton:(id)sender;

/**
 Notifies the view controller when the right button's action has been triggered, manually or by using the keyboard return key.
 @discussion You can override this method to perform additional tasks associated with the right button. You MUST call super at some point in your implementation.
 
 @param sender The object calling this method.
 */
- (void)didPressRightButton:(id)sender NS_REQUIRES_SUPER;

/**
 Verifies if the right button can be pressed. If NO, the button is disabled.
 @discussion You can override this method to perform additional tasks. You SHOULD call super to inherit some conditionals.
 
 @return YES if the right button can be pressed.
 */
- (BOOL)canPressRightButton;

/**
 Notifies the view controller when the user has pasted an image inside of the text view.
 @discussion You can override this method to perform additional tasks associated with image pasting.
 
 @param image The image that has been pasted. Only JPG or PNG are supported.
 */
- (void)didPasteImage:(UIImage *)image DEPRECATED_MSG_ATTRIBUTE("Use -didPasteMediaContent: instead");

/** 
 Notifies the view controller when the user has pasted a supported media content (images and/or videos).
 @discussion You can override this method to perform additional tasks associated with image/video pasting. You don't need to call super since this method doesn't do anything.
 Only supported pastable medias configured in SLKTextView will be forwarded (take a look at SLKPastableMediaType).
 
 @para userInfo The payload containing the media data, content and media types.
 */
- (void)didPasteMediaContent:(NSDictionary *)userInfo;

/**
 Verifies that the typing indicator view should be shown. Default is YES, if meeting some requierements.
 @discussion You can override this method to perform additional tasks. You SHOULD call super to inherit some conditionals.
 
 @return YES if the typing indicator view should be shown.
 */
- (BOOL)canShowTypeIndicator;

/**
 Notifies the view controller when the user has shaked the device for undoing text typing.
 @discussion You can override this method to perform additional tasks associated with the shake gesture. Calling super will prompt a system alert view with undo option. This will not be called if 'undoShakingEnabled' is set to NO and/or if the text view's content is empty.
 */
- (void)willRequestUndo;

/**
 Notifies the view controller when the user has pressed the Return key (↵) with an external keyboard.
 @discussion You can override this method to perform additional tasks. You MUST call super at some point in your implementation.
 */
- (void)didPressReturnKey:(id)sender NS_REQUIRES_SUPER;

/**
 Notifies the view controller when the user has pressed the Escape key (Esc) with an external keyboard.
 @discussion You can override this method to perform additional tasks. You MUST call super at some point in your implementation.
 */
- (void)didPressEscapeKey:(id)sender NS_REQUIRES_SUPER;


///------------------------------------------------
/// @name Text Edition
///------------------------------------------------

/** YES if the text editing mode is active. */
@property (nonatomic, readonly, getter = isEditing) BOOL editing;

/**
 Re-uses the text layout for edition, displaying an accessory view on top of the text input bar with options (cancel & save).
 @discussion You can override this method to perform additional tasks. You MUST call super at some point in your implementation.

 @param text The string text to edit.
 */
- (void)editText:(NSString *)text NS_REQUIRES_SUPER;

/**
 Notifies the view controller when the editing bar's right button's action has been triggered, manually or by using the external keyboard's Return key.
 @discussion You can override this method to perform additional tasks associated with accepting changes. You MUST call super at some point in your implementation.
 
 @param sender The object calling this method.
 */
- (void)didCommitTextEditing:(id)sender NS_REQUIRES_SUPER;

/**
 Notifies the view controller when the editing bar's right button's action has been triggered, manually or by using the external keyboard's Esc key.
 @discussion You can override this method to perform additional tasks associated with accepting changes. You MUST call super at some point in your implementation.
 
 @param sender The object calling this method.
 */
- (void)didCancelTextEditing:(id)sender NS_REQUIRES_SUPER;


///------------------------------------------------
/// @name Text Typing Auto-Completion
///------------------------------------------------

/** The table view used to display autocompletion results. */
@property (nonatomic, readonly) UITableView *autoCompletionView;

/** The recently found prefix symbol used as prefix for autocompletion mode. */
@property (nonatomic, readonly) NSString *foundPrefix;

/** The range of the found prefix in the text view content. */
@property (nonatomic, readonly) NSRange foundPrefixRange;

/** The recently found word at the textView caret position. */
@property (nonatomic, readonly) NSString *foundWord;

/** YES if the autocompletion mode is active. */
@property (nonatomic, readonly, getter = isAutoCompleting) BOOL autoCompleting;

/** An array containing all the registered prefix strings for autocompletion. */
@property (nonatomic, readonly) NSArray *registeredPrefixes;

/**
 Registers any string prefix for autocompletion detection, useful for user mentions and/or hashtags autocompletion.
 @discussion The prefix must be valid NSString (i.e: '@', '#', '\', and so on)
 This also checks if no repeated prefix is inserted.
 
 @param prefixes An array of prefix strings.
 */
- (void)registerPrefixesForAutoCompletion:(NSArray *)prefixes;

/**
 Verifies that the autocompletion view should be shown. Default is NO.
 @discussion You MUST override this method to perform additional tasks, before autocompletion is shown, like populating the data source.
 
 @return YES if the autocompletion view should be shown.
 */
- (BOOL)canShowAutoCompletion;

/**
 Returns a custom height for the autocompletion view. Default is 0.0.
 @discussion You can override this method to return a custom height.

 @return The autocompletion view's height.
 */
- (CGFloat)heightForAutoCompletionView;

/**
 Returns the maximum height for the autocompletion view. Default is 140.0.
 @discussion You can override this method to return a custom max height.

 @return The autocompletion view's max height.
 */
- (CGFloat)maximumHeightForAutoCompletionView;

/**
 Cancels and hides the autocompletion view, animated.
 */
- (void)cancelAutoCompletion;

/** 
 Accepts the autocompletion, replacing the detected key and word with a new string.
 
 @param string The string to be used for replacing autocompletion placeholders.
 */
- (void)acceptAutoCompletionWithString:(NSString *)string;


///------------------------------------------------
/// @name Delegate Methods Requiring Super
///------------------------------------------------

/** UITextViewDelegate */
- (BOOL)textViewShouldBeginEditing:(SLKTextView *)textView NS_REQUIRES_SUPER;
- (BOOL)textViewShouldEndEditing:(SLKTextView *)textView NS_REQUIRES_SUPER;
- (BOOL)textView:(SLKTextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text NS_REQUIRES_SUPER;
- (void)textViewDidChangeSelection:(SLKTextView *)textView NS_REQUIRES_SUPER;

/** UIGestureRecognizerDelegate */
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer NS_REQUIRES_SUPER;

/** UIScrollViewDelegate */
- (void)scrollViewDidScroll:(UIScrollView *)scrollView NS_REQUIRES_SUPER;

/** UIAlertViewDelegate */
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;

@end
