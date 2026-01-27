#include "Edit.h"

Edit::Edit()
    : transport(std::make_shared<Transport>()),
      projectName(""),
      automationManager(),
      videoStartFrame(0),
      video(),
      videoSyncOffset(0),
      editState(),
      undoManager(),
      actionStore(editState, undoManager) {
}
