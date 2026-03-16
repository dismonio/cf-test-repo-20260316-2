// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "Animation.h"
#include <math.h>
#include "globals.h"  // Logging

// Keep track of active animations:
std::map<UIElement*, Animation*> animationsUI;
std::map<int*, Animation*>       animationsInt;
std::list<UIElement*>            tmpAnimationUI;

static int durationWhole = 0; // used in animateAll() for small throttling

/****************************************************
 *  Constructors
 ****************************************************/

Animation::Animation(UIElement* targetUI,
                     AnimationType aniType,
                     int endX, int endY,
                     int totalTime)
{
    this->type = false; // 0 => animating a UIElement
    this->targetUI = targetUI;
    this->targetVal = nullptr;
    this->aniType  = aniType;

    // Capture start
    this->startX   = targetUI->getX();
    this->startY   = targetUI->getY();
    this->startWidth  = targetUI->getWidth();
    this->startHeight = targetUI->getHeight();
    // Skew is optional. If not set, useSkew remains false.

    // End positions
    this->endX     = endX;
    this->endY     = endY;
    // Keep size the same
    this->endWidth  = this->startWidth;
    this->endHeight = this->startHeight;

    this->totalTime = totalTime;
    this->delayTime = 0;
}

Animation::Animation(UIElement* targetUI,
                     AnimationType aniType,
                     int endX, int endY,
                     int endWidth, int endHeight,
                     int totalTime)
{
    this->type = false;
    this->targetUI = targetUI;
    this->targetVal = nullptr;
    this->aniType  = aniType;

    this->startX   = targetUI->getX();
    this->startY   = targetUI->getY();
    this->startWidth  = targetUI->getWidth();
    this->startHeight = targetUI->getHeight();
    // no skew by default

    this->endX   = endX;
    this->endY   = endY;
    this->endWidth   = endWidth;
    this->endHeight  = endHeight;

    this->totalTime = totalTime;
    this->delayTime = 0;
}

Animation::Animation(int* targetVal,
                     AnimationType aniType,
                     int endVal,
                     int totalTime)
{
    this->type = true; // animating an int
    this->targetVal = targetVal;
    this->targetUI  = nullptr;
    this->aniType   = aniType;

    this->startVal  = *targetVal;
    this->endVal    = endVal;
    this->totalTime = totalTime;
    this->delayTime = 0;
}

// Versions with delayTime:
Animation::Animation(UIElement* targetUI,
                     AnimationType aniType,
                     int endX, int endY,
                     int totalTime,
                     int delayTime)
    : Animation(targetUI, aniType, endX, endY, totalTime)
{
    this->delayTime = delayTime;
}
Animation::Animation(UIElement* targetUI,
                     AnimationType aniType,
                     int endX, int endY,
                     int endWidth, int endHeight,
                     int totalTime,
                     int delayTime)
    : Animation(targetUI, aniType, endX, endY, endWidth, endHeight, totalTime)
{
    this->delayTime = delayTime;
}
Animation::Animation(int* targetVal,
                     AnimationType aniType,
                     int endVal,
                     int totalTime,
                     int delayTime)
    : Animation(targetVal, aniType, endVal, totalTime)
{
    this->delayTime = delayTime;
}

/**
 * @brief NEW constructor for animating X/Y/W/H + Skew
 */
Animation::Animation(UIElement* targetUI,
                     AnimationType aniType,
                     int endX, int endY,
                     int endWidth, int endHeight,
                     float endSkew,
                     int totalTime,
                     int delayTime)
{
    this->type = false; // UIElement
    this->targetUI = targetUI;
    this->targetVal = nullptr;
    this->aniType  = aniType;

    // Capture start
    this->startX      = targetUI->getX();
    this->startY      = targetUI->getY();
    this->startWidth  = targetUI->getWidth();
    this->startHeight = targetUI->getHeight();

    // If your UIElement lacks getSkew(), remove or adapt
    this->startSkew   = targetUI->getSkew(); 
    this->useSkew     = true;

    // End
    this->endX      = endX;
    this->endY      = endY;
    this->endWidth  = endWidth;
    this->endHeight = endHeight;
    this->endSkew   = endSkew;

    this->totalTime = totalTime;
    this->delayTime = delayTime;
}

/****************************************************
 *  Initialization
 ****************************************************/

void Animation::init() {
    createTime = millis();
    isStarted  = true;
}

bool Animation::checkTime() {
    return (millis() - createTime >= (unsigned long)delayTime);
}

void Animation::initAni() {
    prevTime    = millis();
    startTime   = millis();
    isAnimating = true;
}

/****************************************************
 *  Getters
 ****************************************************/

bool Animation::getIsStarted()    { return isStarted; }
bool Animation::getIsAnimating()  { return isAnimating; }
bool Animation::getIsFinished()   { return isFinished; }

UIElement* Animation::getTargetElement() { return targetUI; }
int* Animation::getTargetVal()           { return targetVal; }

/****************************************************
 *  Internal Helpers
 ****************************************************/
int Animation::getCurDuration() {
    int curTime = (int)millis();
    return (curTime - prevTime);
}
int Animation::getTotDuration() {
    return totalTime;
}
void Animation::updateTime() {
    prevTime = millis();
}

/****************************************************
 *  Animate Subroutines
 ****************************************************/

void Animation::animateLinear()
{
    if (getCurDuration() > 10 && !isFinished) {
        float t = float(millis() - startTime) / float(totalTime);
        if (t > 1.0f) t = 1.0f;

        if (type == false) {
            // UIElement
            int nxtX = startX + int(t * (endX - startX));
            int nxtY = startY + int(t * (endY - startY));
            int nxtW = startWidth  + int(t * (endWidth  - startWidth));
            int nxtH = startHeight + int(t * (endHeight - startHeight));

            targetUI->setX(nxtX);
            targetUI->setY(nxtY);
            targetUI->setWidth(nxtW);
            targetUI->setHeight(nxtH);

            // If skew is in use:
            if (useSkew) {
                float nxtSkew = startSkew + (t * (endSkew - startSkew));
                targetUI->setSkew(nxtSkew);
            }
        }
        else {
            // int*
            int nxtVal = startVal + int(t * (endVal - startVal));
            *targetVal = nxtVal;
        }

        updateTime();

        // If done, snap to final:
        if (t >= 1.0f) {
            if (!type) {
                targetUI->setX(endX);
                targetUI->setY(endY);
                targetUI->setWidth(endWidth);
                targetUI->setHeight(endHeight);
                if (useSkew) {
                    targetUI->setSkew(endSkew);
                }
            }
            else {
                *targetVal = endVal;
            }
            isFinished = true;
        }
    }
}

void Animation::animateIndent()
{
    if (getCurDuration() > 10 && !isFinished) {
        float t = float(millis() - startTime) / float(totalTime);
        if (t > 1.0f) t = 1.0f;
        // "Indent" uses 1 - 2^(-10*t)
        float easingVal = 1.0f - powf(2.0f, -10.0f * t);

        if (!type) {
            int nxtX = startX + int(easingVal * (endX - startX));
            int nxtY = startY + int(easingVal * (endY - startY));
            int nxtW = startWidth  + int(easingVal * (endWidth  - startWidth));
            int nxtH = startHeight + int(easingVal * (endHeight - startHeight));

            targetUI->setX(nxtX);
            targetUI->setY(nxtY);
            targetUI->setWidth(nxtW);
            targetUI->setHeight(nxtH);

            if (useSkew) {
                float nxtSkew = startSkew + easingVal * (endSkew - startSkew);
                targetUI->setSkew(nxtSkew);
            }
        }
        else {
            int nxtVal = startVal + int(easingVal * (endVal - startVal));
            *targetVal = nxtVal;
        }

        updateTime();
        if (t >= 1.0f) {
            // Snap to final
            if (!type) {
                targetUI->setX(endX);
                targetUI->setY(endY);
                targetUI->setWidth(endWidth);
                targetUI->setHeight(endHeight);
                if (useSkew) {
                    targetUI->setSkew(endSkew);
                }
            }
            else {
                *targetVal = endVal;
            }
            isFinished = true;
        }
    }
}

void Animation::animateIndentInv()
{
    if (getCurDuration() > 10 && !isFinished) {
        float t = float(millis() - startTime) / float(totalTime);
        if (t > 1.0f) t = 1.0f;
        // "IndentInv" uses 2^(10*(t-1))
        float easingVal = powf(2.0f, 10.0f * (t - 1.0f));

        if (!type) {
            int nxtX = startX + int(easingVal * (endX - startX));
            int nxtY = startY + int(easingVal * (endY - startY));
            int nxtW = startWidth  + int(easingVal * (endWidth  - startWidth));
            int nxtH = startHeight + int(easingVal * (endHeight - startHeight));

            targetUI->setX(nxtX);
            targetUI->setY(nxtY);
            targetUI->setWidth(nxtW);
            targetUI->setHeight(nxtH);

            if (useSkew) {
                float nxtSkew = startSkew + easingVal * (endSkew - startSkew);
                targetUI->setSkew(nxtSkew);
            }
        }
        else {
            int nxtVal = startVal + int(easingVal * (endVal - startVal));
            *targetVal = nxtVal;
        }

        updateTime();
        if (t >= 1.0f) {
            if (!type) {
                targetUI->setX(endX);
                targetUI->setY(endY);
                targetUI->setWidth(endWidth);
                targetUI->setHeight(endHeight);
                if (useSkew) {
                    targetUI->setSkew(endSkew);
                }
            }
            else {
                *targetVal = endVal;
            }
            isFinished = true;
        }
    }
}

void Animation::animateBounce()
{
    if (getCurDuration() > 10 && !isFinished) {
        float t = float(millis() - startTime) / float(totalTime);
        if (t > 1.0f) t = 1.0f;

        // same bounce formula as your original
        float easingVal = powf(2.0f, -10.0f * t) *
                          sinf((t * 10.0f - 0.75f) * (2.0f * float(M_PI) / 3.0f)) +
                          1.0f;

        if (!type) {
            int nxtX = startX + int(easingVal * (endX - startX));
            int nxtY = startY + int(easingVal * (endY - startY));
            int nxtW = startWidth  + int(easingVal * (endWidth  - startWidth));
            int nxtH = startHeight + int(easingVal * (endHeight - startHeight));

            targetUI->setX(nxtX);
            targetUI->setY(nxtY);
            targetUI->setWidth(nxtW);
            targetUI->setHeight(nxtH);

            if (useSkew) {
                float nxtSkew = startSkew + easingVal * (endSkew - startSkew);
                targetUI->setSkew(nxtSkew);
            }
        }
        else {
            int nxtVal = startVal + int(easingVal * (endVal - startVal));
            *targetVal = nxtVal;
        }

        updateTime();
        if (t >= 1.0f) {
            if (!type) {
                targetUI->setX(endX);
                targetUI->setY(endY);
                targetUI->setWidth(endWidth);
                targetUI->setHeight(endHeight);
                if (useSkew) {
                    targetUI->setSkew(endSkew);
                }
            }
            else {
                *targetVal = endVal;
            }
            isFinished = true;
        }
    }
}

/**
 * @brief Our new "PARALLELOGRAM" method, using an "easeInOutBack" style approach
 *        to animate X, Y, W, H, plus skew in a fun "cyberpunk" style.
 */
void Animation::animateParallelogram()
{
    if (getCurDuration() > 10 && !isFinished) {
        float t = float(millis() - startTime) / float(totalTime);
        if (t > 1.0f) t = 1.0f;

        // easeInOutBack approach:
        float c1 = 1.70158f;
        float c2 = c1 * 1.525f;
        float easingVal;
        if (t < 0.5f) {
            float tw = 2.0f * t;
            easingVal = 0.5f * (tw * tw * ((c2 + 1.0f) * tw - c2));
        } else {
            float tw = (2.0f * t - 2.0f);
            easingVal = 0.5f * (tw * tw * ((c2 + 1.0f) * tw + c2) + 2.0f);
        }

        if (!type) {
            int nxtX = startX + int(easingVal * (endX - startX));
            int nxtY = startY + int(easingVal * (endY - startY));
            int nxtW = startWidth  + int(easingVal * (endWidth  - startWidth));
            int nxtH = startHeight + int(easingVal * (endHeight - startHeight));
            targetUI->setX(nxtX);
            targetUI->setY(nxtY);
            targetUI->setWidth(nxtW);
            targetUI->setHeight(nxtH);

            if (useSkew) {
                float nxtSkew = startSkew + (easingVal * (endSkew - startSkew));
                targetUI->setSkew(nxtSkew);
            }
        }
        else {
            // If type=1 => int*, we won't do a parallelogram. 
            // Or we could do the same approach for an int. 
            // But typically "PARALLELOGRAM" is for UIElements.
            int nxtVal = startVal + int(easingVal * (endVal - startVal));
            *targetVal = nxtVal;
        }

        updateTime();
        if (t >= 1.0f) {
            if (!type) {
                targetUI->setX(endX);
                targetUI->setY(endY);
                targetUI->setWidth(endWidth);
                targetUI->setHeight(endHeight);
                if (useSkew) {
                    targetUI->setSkew(endSkew);
                }
            }
            else {
                *targetVal = endVal;
            }
            isFinished = true;
        }
    }
}

/****************************************************
 *  animate()  (Called each frame or ~10ms)
 ****************************************************/
void Animation::animate()
{
    switch (this->aniType) {
        case LINEAR:
            animateLinear();
            break;
        case INDENT:
            animateIndent();
            break;
        case INDENTINV:
            animateIndentInv();
            break;
        case BOUNCE:
            animateBounce();
            break;
        case PARALLELOGRAM: //
            animateParallelogram();
            break;
        default:
            // default to linear if unknown
            animateLinear();
            break;
    }
}

/****************************************************
 *  Animation Management
 ****************************************************/

/**
 * @brief Insert a new animation. 
 *        If there's already an animation on the same target, we replace it.
 */
void insertAnimation(Animation* animation)
{
    static int getTargetType = 0; // 0=init, 1=UIElement, 2=int*
    // If it’s animating a UIElement
    if (animation->getTargetElement() != nullptr) {
        UIElement* tgt = animation->getTargetElement();
        // If there’s an existing animation for that UIElement, remove it
        auto it = animationsUI.find(tgt);
        if (it != animationsUI.end()) {
            delete it->second;       // free the old animation
            animationsUI.erase(it);  // remove from map
        }
        // Insert the new one
        animationsUI[tgt] = animation;
        getTargetType = 1; // UIElement
    }
    else {
        // Otherwise, it’s an int*
        int* valPtr = animation->getTargetVal();
        auto it = animationsInt.find(valPtr);
        if (it != animationsInt.end()) {
            delete it->second;
            animationsInt.erase(it);
        }
        animationsInt[valPtr] = animation;
        getTargetType = 2; // int*
    }
    // ESP_LOGI(TAG_MAIN, "Inserted UIElement* animation for %p", animation->getTargetElement());
    // ESP_LOGI(TAG_MAIN, "Inserted int* for %p", animation->getTargetVal());
    // ESP_LOGI(TAG_MAIN, "AnimationsUI size = %d", animationsUI.size());
    // ESP_LOGI(TAG_MAIN, "getTargetType = %d", getTargetType);
}

/**
 * @brief Check if an int* is done animating (i.e. not in the map).
 */
bool isFinished(int* targetInt)
{
    return (animationsInt.find(targetInt) == animationsInt.end());
}

/**
 * @brief Check if a UIElement* is done animating.
 */
bool isFinished(UIElement* targetUI)
{
    return (animationsUI.find(targetUI) == animationsUI.end());
}

/**
 * @brief Add a UIElement* to tmpAnimationUI so we can remove it after animations.
 */
void insertTmpAnimationPointer(UIElement* tmpUI)
{
    tmpAnimationUI.push_back(tmpUI);
}

/**
 * @brief animateAll() is called in your main loop. It updates all active animations
 *        if enough time has passed since the last update.
 */
void animateAll()
{
    // For example, throttle to every 5ms
    if (millis() - durationWhole < 5) return;
    durationWhole = millis();

    // Update UIElement animations
    for (auto it = animationsUI.begin(); it != animationsUI.end(); ) {
        Animation* ani = it->second;
        if (!ani->getIsFinished()) {
            if (!ani->getIsStarted()) {
                ani->init();
            }
            else if (ani->checkTime() && !ani->getIsAnimating()) {
                ani->initAni();
            }
            if (ani->getIsAnimating()) {
                ani->animate();
            }
            ++it;
        }
        else {
            // done, remove
            it = animationsUI.erase(it);
        }
    }

    // Update int* animations
    for (auto it = animationsInt.begin(); it != animationsInt.end(); ) {
        Animation* ani = it->second;
        if (!ani->getIsFinished()) {
            if (!ani->getIsStarted()) {
                ani->init();
            }
            else if (ani->checkTime() && !ani->getIsAnimating()) {
                ani->initAni();
            }
            if (ani->getIsAnimating()) {
                ani->animate();
            }
            ++it;
        }
        else {
            // done, remove
            it = animationsInt.erase(it);
        }
    }
}

/**
 * @brief Removes ephemeral UIElements from tmpAnimationUI if they’re finished animating.
 *        Typically called each frame too, right after animateAll().
 */
void updateTmp()
{
    for (auto it = tmpAnimationUI.begin(); it != tmpAnimationUI.end(); ) {
        if (!isFinished(*it)) {
            ++it;
        } else {
            it = tmpAnimationUI.erase(it);
        }
    }
}

void Animation::updateToCurrentValue()
{
    // If not even started, we must init (this sets createTime, etc.)
    if (!isStarted)
    {
        init();  // sets isStarted=true, createTime=millis()
    }

    // If we haven't passed the delayTime yet, there is no partial progress
    if (!checkTime())
    {
        // i.e. the animation is still waiting for its delay to expire,
        // so the target remains at the start.
        return;
    }

    // If it hasn’t begun the actual motion yet, we do that now
    if (!isAnimating)
    {
        initAni(); // sets startTime=millis(), isAnimating=true
    }

    // Now compute how far along (0..1) we *should* be
    unsigned long now  = millis();
    float elapsed = float(now - startTime);
    float t       = (totalTime == 0) ? 1.0f : (elapsed / float(totalTime));
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;

    // We will use the same easing formula as animate(),
    // but we skip the "if (getCurDuration()>10)" throttle. We want an immediate update.
    float easingVal = 0.f;
    switch (aniType)
    {
        case LINEAR:
            easingVal = t;
            break;
        case INDENT: // "ease-out" style
            // 1.0 - 2^(-10 * t)
            easingVal = 1.0f - powf(2.f, -10.f * t);
            break;
        case INDENTINV: // "ease-in" style
            // 2^(10*(t-1))
            easingVal = powf(2.f, 10.f * (t - 1.f));
            break;
        case BOUNCE:
        {
            // same bounce formula from animateBounce()
            float bounce = powf(2.f, -10.f * t) *
                           sinf((t * 10.f - 0.75f) * (2.f * float(M_PI) / 3.f)) + 1.f;
            easingVal = bounce;
            break;
        }
        case PARALLELOGRAM: // "easeInOutBack"
        {
            float c1 = 1.70158f;
            float c2 = c1 * 1.525f;
            if (t < 0.5f)
            {
                float tw = 2.f * t;
                easingVal = 0.5f * (tw * tw * ((c2 + 1.f) * tw - c2));
            }
            else
            {
                float tw = (2.f * t - 2.f);
                easingVal = 0.5f * (tw * tw * ((c2 + 1.f) * tw + c2) + 2.f);
            }
            break;
        }
        default:
            // fallback to linear
            easingVal = t;
            break;
    }

    // Update the actual target:
    if (!type) // UIElement
    {
        int newX = startX + int(easingVal * (endX - startX));
        int newY = startY + int(easingVal * (endY - startY));
        int newW = startWidth  + int(easingVal * (endWidth  - startWidth));
        int newH = startHeight + int(easingVal * (endHeight - startHeight));
        targetUI->setX(newX);
        targetUI->setY(newY);
        targetUI->setWidth(newW);
        targetUI->setHeight(newH);

        if (useSkew)
        {
            float newSkew = startSkew + easingVal * (endSkew - startSkew);
            targetUI->setSkew(newSkew);
        }
    }
    else // animating an int*
    {
        int newVal = startVal + int(easingVal * (endVal - startVal));
        *targetVal = newVal;
    }

    // If t is fully at 1.0 => we’re finished, snap exactly to end
    if (t >= 1.0f)
    {
        if (!type)
        {
            targetUI->setX(endX);
            targetUI->setY(endY);
            targetUI->setWidth(endWidth);
            targetUI->setHeight(endHeight);
            if (useSkew)
            {
                targetUI->setSkew(endSkew);
            }
        }
        else
        {
            *targetVal = endVal;
        }
        isFinished = true;
    }
}
