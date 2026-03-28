#ifndef INCLUDED_KEYLOCATIONWIDGET_H
#define INCLUDED_KEYLOCATIONWIDGET_H

#include "ui/action.h"
#include "ui/widget.h"
#include "ui/uitimers.h"
#include "types/strbuf.h"
#include "keylocation.h"

class KeyHardware;
class Timer;

class KeyLocationWidget : public Widget
{
public:
    KeyLocationWidget(KeyHardware&  keyHardware);

public:
    virtual bool processEvent(const Event& inEvent) override;
    virtual void setFocused(bool nFocused) override;
    virtual void render(const RasterLine& rasterLine, int row) override;
    virtual Dimension minimumSize() const override;

public:
    void clear();

private:
    void readNextKeyPress();

public:
    KeyLocation location;
    Action      locationSelected;

private:
    KeyHardware& mKeyHardware;
    StrBuf<24>   mLocationStr;
    bool         mLocationSet;
    bool         mFocused;
    bool         mFlash;
    bool         mTrigger;
};

#endif
