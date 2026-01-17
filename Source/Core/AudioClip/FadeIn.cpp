#include "FadeIn.h"

FadeIn::FadeIn() : endSample(0) {
}

void FadeIn::remove() {
    isActive = false;
}