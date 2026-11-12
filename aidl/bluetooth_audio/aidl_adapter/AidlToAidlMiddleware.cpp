/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "BtAudioNakahara"

#include <android-base/logging.h>
#include <android/binder_ibinder_platform.h>

#include "AidlToAidlMiddleware.h"

namespace vendor {
namespace mediatek {
namespace hardware {
namespace bluetooth {
namespace audio {
namespace adapter {

AidlToAidlMiddleware::AidlToAidlMiddleware(
        const std::shared_ptr<::aidl::android::hardware::bluetooth::audio::IBluetoothAudioPort>&
                port)
    : port(port) {}

ndk::ScopedAStatus AidlToAidlMiddleware::startStream(bool is_low_latency) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    return port->startStream(is_low_latency);
}

ndk::ScopedAStatus AidlToAidlMiddleware::suspendStream() {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    return port->suspendStream();
}

ndk::ScopedAStatus AidlToAidlMiddleware::stopStream() {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    return port->stopStream();
}

ndk::ScopedAStatus AidlToAidlMiddleware::getPresentationPosition(
        mtk::PresentationPosition* _aidl_return) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    PresentationPosition aosp_return;
    auto retval = port->getPresentationPosition(&aosp_return);
    if (retval.isOk()) {
        _aidl_return->remoteDeviceAudioDelayNanos = aosp_return.remoteDeviceAudioDelayNanos;
        _aidl_return->transmittedOctets = aosp_return.transmittedOctets;
        _aidl_return->transmittedOctetsTimestamp.tvSec =
                aosp_return.transmittedOctetsTimestamp.tvSec;
        _aidl_return->transmittedOctetsTimestamp.tvNSec =
                aosp_return.transmittedOctetsTimestamp.tvNSec;
    }
    return retval;
}

ndk::ScopedAStatus AidlToAidlMiddleware::updateSourceMetadata(
        const SourceMetadata& source_metadata) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    return port->updateSourceMetadata(source_metadata);
}

ndk::ScopedAStatus AidlToAidlMiddleware::updateSinkMetadata(const SinkMetadata& sink_metadata) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    return port->updateSinkMetadata(sink_metadata);
}

ndk::ScopedAStatus AidlToAidlMiddleware::setLatencyMode(mtk::LatencyMode latency_mode) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    LatencyMode aosp_mode = LatencyMode::UNKNOWN;
    switch (latency_mode) {
        case mtk::LatencyMode::LOW_LATENCY:
            aosp_mode = LatencyMode::LOW_LATENCY;
            break;
        case mtk::LatencyMode::FREE:
            aosp_mode = LatencyMode::FREE;
            break;
        case mtk::LatencyMode::UNKNOWN:
        default:
            break;
    }
    return port->setLatencyMode(aosp_mode);
}

ndk::ScopedAStatus AidlToAidlMiddleware::enterGameMode(int8_t enter) {
    if (!port) return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    LOG(DEBUG) << __func__ << ": mapping game mode (" << static_cast<int>(enter)
               << ") to LatencyMode";
    LatencyMode aosp_mode = (enter != 0) ? LatencyMode::LOW_LATENCY : LatencyMode::FREE;
    return port->setLatencyMode(aosp_mode);
}

ndk::SpAIBinder AidlToAidlMiddleware::createBinder() {
    auto binder = mtk::BnBluetoothAudioPort::createBinder();
    AIBinder_setInheritRt(binder.get(), true);
    return binder;
}

}  // namespace adapter
}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor
