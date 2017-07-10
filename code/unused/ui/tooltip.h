#include "base/types.h"
#include "base/defines.h"
#include "gui/widget.h"

namespace engine
{
class OsWindow;

class E_API ToolTip : public Widget
{
public:
protected:
	OsWindow* m_pToolTipWnd;
};


}
