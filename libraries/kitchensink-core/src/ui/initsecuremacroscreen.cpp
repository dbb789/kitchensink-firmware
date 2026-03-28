#include "ui/initsecuremacroscreen.h"

#include "event/screenevent.h"
#include "keyboardstateutil.h"
#include "config.h"

InitSecureMacroScreen::InitSecureMacroScreen(SecureMacroSet& secureMacroSet,
                                             UITimers&       uiTimers,
                                             EventStage&     next)
    : mSecureMacroSet(secureMacroSet)
    , mUITimers(uiTimers)
    , mPasswordEntry("Password",
                     60,
                     PasswordEntryWidget(uiTimers))
    , mConfirmEntry("Confirm",
                    60,
                    PasswordEntryWidget(uiTimers))
    , mStatusLabel(" Existing data will be erased ", Justify::kCenter)
    , mItems({{ mPasswordEntry,
                mConfirmEntry,
                mStatusLabel }})
    , mHStackWidget(mItems, true)
    , mNext(next)
{
    mHStackWidget.applied = Action::memFn<InitSecureMacroScreen,
                                          &InitSecureMacroScreen::onApply>(this);
}

bool InitSecureMacroScreen::processEvent(const Event& event)
{
    if (mUITimers.flashTimer().matches(event))
    {
        mStatusLabel.invert = !mStatusLabel.invert;
        mStatusLabel.invalidateWidget();
        return true;
    }
        
    return mHStackWidget.processEvent(event);
}

Widget& InitSecureMacroScreen::rootWidget()
{
    return mHStackWidget;
}

void InitSecureMacroScreen::onApply()
{
    StrRef passwordA(mPasswordEntry.widget.password);
    StrRef passwordB(mConfirmEntry.widget.password);

    if (!passwordA.empty() && !passwordB.empty())
    {
        mStatusLabel.invert = false;
        
        if (passwordA != passwordB)
        {
            mStatusLabel.text = "Passwords do not match";
            mStatusLabel.invalidateWidget();
        }
        else if (passwordA.length() < Config::kPasswordMin)
        {
            mStatusLabel.text = "Password too short";
            mStatusLabel.invalidateWidget();
        }
        else if (passwordA.length() > Config::kPasswordMax)
        {
            mStatusLabel.text = "Password too long";
            mStatusLabel.invalidateWidget();
        }
        else
        {
            mSecureMacroSet.lock();
            mSecureMacroSet.password = passwordA;
            
            mNext.processEvent(ScreenEvent::create(ScreenEvent::Type::kScreen,
                                                   ScreenEvent::kMacroSave));
        }
    }
}
