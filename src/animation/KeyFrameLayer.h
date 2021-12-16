//
// Created by Tobiathan on 11/14/21.
//

#ifndef SENIORRESEARCH_KEYFRAMELAYER_H
#define SENIORRESEARCH_KEYFRAMELAYER_H

#include "../util/Includes.h"
#include "KeyFrame.h"
#include "../vendor/glm/vec2.hpp"
#include "../gl/Mesh2D.h"
#include <functional>

template <class T>
class KeyFrameLayer {
public:
    struct KeyFrameSet {
        KeyFrame<T>& frame1;
        KeyFrame<T>& frame2;

        bool IsEither(const KeyFrame<T>& frame) const {
            return frame == frame1 || frame == frame2;
        }
    };


    void RemoveAtTime(float time) {
        for (int i = 0; i < frames.size(); i++) {
            if (frames[i].time == time) {
                frames.erase(frames.begin() + i);
                return;
            }
        }
    }

    static float RowToHeight(int row) {
        return 0.9f - (float) row * 0.2f - 0.4f;
    }

    void Render(Mesh2D& canvas, int line, float time, std::function<bool(KeyFrame<T>*)> containsFunc) {
        const auto SideToSideLine = [&](float height) {
            canvas.AddLines({{-1.0f, height}, {1.0f, height}}, {0.5f, 0.5f, 0.5f, 1.0f}, 0.003f);
            canvas.AddLines({{-1.0f, height}, {1.0f, height}}, {0.5f, 0.5f, 0.5f, 1.0f}, 0.003f);
        };

        if (!frames.empty()) {
            auto upper = std::upper_bound(frames.begin(), frames.end(), time,
                                          [](float value, const KeyFrame<T>& frame) {
                                              return value < frame.time;
                                          }
            );
            const KeyFrame<T>& frame1 = [&]{
                if (upper == frames.begin()) return upper[0];
                if (upper == frames.end()) return frames.back();
                return upper[-1];
            }();
            const KeyFrame<T>& frame2 = [&]{
                if (upper == frames.end()) return frames.back();
                return upper[0];
            }();

            float t = (time - frame1.time) / (frame2.time - frame1.time); // Linear
            // TODO: calc using blendmodes!

            for (int i = 0; i < frames.size(); i++) {
                auto& frame = frames[i];
                bool selected = (frame.time == frame1.time || frame.time == frame2.time);

                canvas.AddLines({{frame.time, RowToHeight(line) - 0.1f}, {frame.time, RowToHeight(line) + 0.1f}}, {0.43f, 0.43f, 0.43f, 1.0f}, 0.003f);

                glm::vec4 color = [&] {
                    if (containsFunc(&frame)) return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
                    if (frame.time == frame1.time && frame.time == frame2.time) return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
                    if (frame.time == frame1.time) return (glm::vec4(1.0f, 0.5f, 0.0f, 1.0f) * (1.0f - t) + glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * t);
                    if (frame.time == frame2.time) return (glm::vec4(1.0f, 0.5f, 0.0f, 1.0f) * t + glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (1.0f - t));
                    return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                }();
                canvas.AddPolygonOutline({frame.time, RowToHeight(line)}, 0.03f, frame.blendModeID == 0 ? 3 : 10, color);
                if (i != frames.size() - 1) {
                    glm::vec2 p1 = glm::vec2(frame.time, RowToHeight(line) - 0.1f);
                    glm::vec2 p2 = glm::vec2(frames[i + 1].time, RowToHeight(line) + 0.1f);

                    bool onLine = (frame.time == frame1.time && frame.time != frame2.time);
                    glm::vec4 lineColor = onLine ? glm::vec4(0.85f, 1.0f, 0.85f, 1.0f) : glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);

                    const auto GetGraphPosAtT = [&](float t, const KeyFrame<T>& frame1, const KeyFrame<T>& frame2) {
                        return p1 + glm::vec2(t, KeyFrame<T>::RemapTime(t, frame1, frame2)) * (p2 - p1);
                    };

                    {
                        int pCount = std::round((frames[i + 1].time - frame.time) * 10.0f) * 4 + 1;
                        std::vector<glm::vec2> vec;
                        vec.reserve(pCount);

                        for (int drawP = 0; drawP <= pCount; drawP++) {
                            if (drawP == pCount) {
                                vec.push_back({p2});
                                continue;
                            }
                            vec.push_back(GetGraphPosAtT((float) drawP / (float) pCount, frame, frames[i + 1]));
                        }

                        canvas.AddLines(vec, lineColor, 0.003f);
                    }

                    if (onLine) {
                        canvas.AddPolygonOutline(GetGraphPosAtT(t, frame, frames[i + 1]), 0.013f, 6, lineColor);
                    }
                }
            }

        }
        SideToSideLine(RowToHeight(line) - 0.1f);
        SideToSideLine(RowToHeight(line) + 0.1f);
    }

    bool HasValue() {
        return !frames.empty();
    }

    T GetAnimatedVal(float time) {
        auto upper = std::upper_bound(frames.begin(), frames.end(), time,
          [](float value, const KeyFrame<T>& frame) {
              return value < frame.time;
          }
        );
        const KeyFrame<T>& frame1 = [&]{
            if (upper == frames.begin()) return upper[0];
            if (upper == frames.end()) return frames.back();
            return upper[-1];
        }();
        const KeyFrame<T>& frame2 = [&]{
            if (upper == frames.end()) return frames.back();
            return upper[0];
        }();

        float t = (time - frame1.time) / (frame2.time - frame1.time); // Linear

        if (frame1.time == frame2.time) {
            t = -1.0f;
        } else {
            t = KeyFrame<T>::RemapTime(t, frame1, frame2);
        }

        return KeyFrame<T>::GetAnimatedValueAtT(frame1, frame2, t);
    }

    void Insert(KeyFrame<T> frame) {
        const float time = frame.time;

        for (int i = 0; i < frames.size(); i++) {
            float otherTime = frames[i].time;
            if (time == otherTime) {
                frames[i] = frame;
            }  else if (time < otherTime) {
                frames.insert(frames.begin() + i, {frame});
            } else {
                continue;
            }
            return;
        }
        frames.push_back(frame);
    }

    std::optional<KeyFrame<T>*> KeyFrameBelow(float time) {
        auto upper = std::upper_bound(frames.begin(), frames.end(), time,
                                      [](float value, const KeyFrame<T>& frame) {
                                          return frame.time >= value;
                                      }
        );
        if (upper == frames.end()) {
            if (!frames.empty()) return std::make_optional(&frames.back());
            return std::nullopt;
        }
        if (upper[0].time == frames.front().time) return std::nullopt;

        return std::make_optional(&upper[-1]);
    }

    std::optional<KeyFrame<T>*> KeyFrameAbove(float time) {
        auto upper = std::upper_bound(frames.begin(), frames.end(), time,
            [](float value, const KeyFrame<T>& frame) {
              return frame.time > value;
            }
        );
        if (upper == frames.end()) return std::nullopt;

        return std::make_optional(&upper[0]);
    }

    std::vector<KeyFrame<T>> frames;
};


#endif //SENIORRESEARCH_KEYFRAMELAYER_H
