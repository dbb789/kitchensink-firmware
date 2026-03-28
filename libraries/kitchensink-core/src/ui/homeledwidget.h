#ifndef INCLUDED_HOMELEDWIDGET_H
#define INCLUDED_HOMELEDWIDGET_H

#include "timer/timermanager.h"
#include "ui/widget.h"
#include "ui/action.h"
#include "ui/uitimers.h"
#include "types/strbuf.h"
#include "homeled.h"

class HomeLedWidget : public Widget
{
public:
    HomeLedWidget(const SmartKeySet& smartKeySet,
                  UITimers&          uiTimers);

    HomeLedWidget(HomeLedWidget&&) = default;

public:
    virtual bool processEvent(const Event& inEvent) override;
    virtual void setFocused(bool nFocused) override;
    virtual void render(const RasterLine& rasterLine, int row) override;
    virtual Dimension minimumSize() const override;

public:
    void update();
    
public:
    HomeLed homeLed;

private:
    const SmartKeySet& mSmartKeySet;
    UITimers&          mUITimers;
    bool               mFocused;
    bool               mFlash;
    bool               mTrigger;

private:
    HomeLedWidget(const HomeLedWidget&) = delete;
    HomeLedWidget& operator=(const HomeLedWidget&) = delete;
};

#endif
