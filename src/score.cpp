#include "score.hpp"
#include "random.hpp"
#include <algorithm>
#include <cmath>

static ScorePoint p(float x, float y) { return {x,y}; }
static void addStroke(Score& s, MarkKind k, std::initializer_list<ScorePoint> pts, float w=2, bool filled=false) { s.strokes.push_back({k, pts, w, filled}); }

static Staff generateStaff(Score& score, Random& r, float y, const ScoreParameters& q) {
    Staff staff{y, r.range(9, 15), {}};
    for (int line = 0; line < 5; ++line) {
        std::vector<ScorePoint> points;
        float drift = r.range(-q.staffDistortion, q.staffDistortion) * 8;
        for (int x = 70; x < score.width - 50; x += 55) {
            float bend = std::sin(x * .009f + line) * q.chaos * 5 + r.range(-drift, drift);
            points.push_back(p(static_cast<float>(x), y + line * staff.spacing + bend));
        }
        // TINKER HERE: increase the probability that staff lines terminate early.
        if (r.chance(q.lineBreak)) points.resize(static_cast<size_t>(r.integer(3, static_cast<int>(points.size()))));
        staff.lines.push_back({MarkKind::Connector, points, r.range(1, 3), false});
    }
    return staff;
}

static void generateNoteCluster(Score& score, const Staff& staff, Random& r, const ScoreParameters& q) {
    int count = std::max(2, static_cast<int>(q.density * r.range(5, 14)));
    for (int i=0; i<count; ++i) {
        // TINKER HERE: widen this range to push more notes outside the staff.
        float x = r.range(100, score.width - 80), y = staff.y + r.range(-18, 5 * staff.spacing + 18);
        float size = r.range(4, 10) * (r.chance(q.chaos) ? r.range(.7f, 1.8f) : 1.0f);
        addStroke(score, MarkKind::Note, {p(x-size,y),p(x+size,y),p(x,y-size*.7f),p(x-size,y)}, 2, true);
        // TINKER HERE: change the stem range to make the clusters more vertical.
        float stem = r.range(-45, 48);
        addStroke(score, MarkKind::EventLine, {p(x+size*.6f,y),p(x+size*.6f,y+stem)}, r.range(1,3));
        // TINKER HERE: increase this probability for more articulation debris.
        if (r.chance(.2f*q.density)) addStroke(score, MarkKind::Dot, {p(x+size*1.7f,y-3),p(x+size*2.5f,y-3)}, 4, true);
    }
    // TINKER HERE: make noteheads merge more aggressively with staff lines.
    if (r.chance(.22f*q.density)) addStroke(score, MarkKind::Connector, {p(r.range(120,score.width-180), staff.y+staff.spacing),p(r.range(150,score.width-80), staff.y+staff.spacing*3)}, 2);
}

static void generateAnnotation(Score& score, Random& r, const ScoreParameters& q) {
    // TINKER HERE: constrain annotation coordinates to create intentional zones.
    ScorePoint a{r.range(80,score.width-100), r.range(55,score.height-55)};
    const MarkKind kinds[] = {MarkKind::Rest,MarkKind::Arc,MarkKind::Bracket,MarkKind::Box,MarkKind::Arrow,MarkKind::Circle,MarkKind::Rehearsal,MarkKind::Number,MarkKind::Duration,MarkKind::Instruction};
    // TINKER HERE: bias the mark vocabulary toward boxes, arrows, or text.
    MarkKind k = kinds[r.integer(0,9)];
    if (k == MarkKind::Rest) addStroke(score,k,{p(a.x,a.y),p(a.x+18,a.y-18),p(a.x+9,a.y+17)},2);
    else if (k == MarkKind::Arc) addStroke(score,k,{p(a.x-25,a.y+8),p(a.x,a.y-22),p(a.x+25,a.y+8)},2);
    else if (k == MarkKind::Bracket) addStroke(score,k,{p(a.x,a.y-28),p(a.x-12,a.y-28),p(a.x-12,a.y+28),p(a.x,a.y+28)},2);
    else if (k == MarkKind::Box) addStroke(score,k,{p(a.x-20,a.y-15),p(a.x+22,a.y-15),p(a.x+22,a.y+17),p(a.x-20,a.y+17),p(a.x-20,a.y-15)},2);
    else if (k == MarkKind::Arrow) { addStroke(score,k,{p(a.x-30,a.y+20),p(a.x+28,a.y-17)},2); addStroke(score,k,{p(a.x+28,a.y-17),p(a.x+11,a.y-14)},2); }
    else if (k == MarkKind::Circle) addStroke(score,k,{p(a.x-19,a.y),p(a.x-14,a.y-15),p(a.x,a.y-21),p(a.x+14,a.y-15),p(a.x+19,a.y),p(a.x+14,a.y+15),p(a.x,a.y+21),p(a.x-14,a.y+15),p(a.x-19,a.y)},2);
    else { const char* words[] = {"hold", "unmake", "dry", "x3", "17", "slow?"}; score.texts.push_back({k,a,words[r.integer(0,5)],r.range(12,27),r.range(-18,18)}); }
    // TINKER HERE: change the density of invented performance instructions.
    (void)q;
}

static void generateConnector(Score& score, Random& r) { // TINKER HERE: make connectors cross more staff systems.
    addStroke(score,MarkKind::Connector,{p(r.range(80,score.width*.45f),r.range(80,score.height-80)),p(r.range(score.width*.55f,score.width-80),r.range(80,score.height-80))},r.range(1,4)); }
static void generateOversizedMark(Score& score, Random& r) { ScorePoint a{r.range(180,score.width-180),r.range(130,score.height-130)}; float z=r.range(35,95); addStroke(score,MarkKind::Oversized,{p(a.x-z,a.y),p(a.x,a.y-z),p(a.x+z,a.y),p(a.x,a.y+z),p(a.x-z,a.y)},r.range(3,8)); }

Score generateScore(const ScoreParameters& q) {
    Score score; score.width=q.width; score.height=q.height; Random r(q.seed);
    int systems = r.integer(3,8); float usable = score.height / static_cast<float>(systems+1);
    for (int i=0;i<systems;++i) { Staff staff=generateStaff(score,r,usable*(i+1),q); score.staffs.push_back(staff); generateNoteCluster(score,staff,r,q); }
    int annotations=static_cast<int>(score.width/180*q.density);
    for (int i=0;i<annotations;++i) if (r.chance(q.annotation)) generateAnnotation(score,r,q);
    if (r.chance(.8f)) generateConnector(score,r);
    // TINKER HERE: make oversized symbols rare landmarks or recurring interruptions.
    if (r.chance(q.oversized)) generateOversizedMark(score,r);
    return score;
}
