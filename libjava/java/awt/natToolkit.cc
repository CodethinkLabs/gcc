#include <config.h>

/*#define ENABLE_GTK*/

#include <cni.h>
#include <java/awt/Toolkit.h>
#ifdef ENABLE_GTK
#include <java/awt/peer/GtkToolkit.h>
#endif

void
java::awt::Toolkit::init()
{
#ifdef ENABLE_GTK
  defaultToolkit = new java::awt::peer::GtkToolkit();
#else
  JvFail("no awt (graphics) toolkit available");
#endif
}
