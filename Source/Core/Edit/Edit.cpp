#include "Edit.h"

Edit::Edit()
    : transport(),
      projectName(""),
      automationManager(),
      videoStartFrame(0),
      video(),
      videoSyncOffset(0),
      editState(),
      undoManager(),
      actionStore(editState, undoManager) {
}
