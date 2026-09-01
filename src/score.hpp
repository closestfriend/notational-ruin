#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct ScorePoint { float x = 0, y = 0; };
enum class MarkKind { Note, Rest, Arc, Dot, EventLine, Bracket, Box, Arrow, Circle, Connector, Rehearsal, Number, Duration, Instruction, Oversized };
struct Stroke { MarkKind kind; std::vector<ScorePoint> points; float width = 2; bool filled = false; };
struct TextMark { MarkKind kind; ScorePoint position; std::string text; float size = 18; float angle = 0; };
struct Staff { float y = 0, spacing = 12; std::vector<Stroke> lines; };
struct Score { int width = 1600, height = 1000; std::vector<Staff> staffs; std::vector<Stroke> strokes; std::vector<TextMark> texts; };
struct ScoreParameters { int width = 1600, height = 1000; uint32_t seed = 18; float density = .7f, chaos = .4f, staffDistortion = .5f, collision = .28f, annotation = .32f, lineBreak = .13f, oversized = .08f; };

Score generateScore(const ScoreParameters& parameters);
