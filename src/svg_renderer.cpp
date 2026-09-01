#include "svg_renderer.hpp"
#include <sstream>
#include <iomanip>
static void points(std::ostringstream& out, const std::vector<ScorePoint>& ps) { for (const auto& q:ps) out<<q.x<<','<<q.y<<' '; }
std::string renderSvg(const Score& s) {
    std::ostringstream o; o<<std::fixed<<std::setprecision(1)<<"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\""<<s.width<<"\" height=\""<<s.height<<"\" viewBox=\"0 0 "<<s.width<<' '<<s.height<<"\"><rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    for (const auto& staff:s.staffs) for (const auto& l:staff.lines) { o<<"<polyline fill=\"none\" stroke=\"black\" stroke-width=\""<<l.width<<"\" points=\""; points(o,l.points); o<<"\"/>\n"; }
    for (const auto& x:s.strokes) { o<<"<polyline fill=\""<<(x.filled?"black":"none")<<"\" stroke=\"black\" stroke-width=\""<<x.width<<"\" stroke-linecap=\"round\" points=\""; points(o,x.points); o<<"\"/>\n"; }
    for (const auto& t:s.texts) o<<"<text x=\""<<t.position.x<<"\" y=\""<<t.position.y<<"\" font-size=\""<<t.size<<"\" fill=\"black\" transform=\"rotate("<<t.angle<<' '<<t.position.x<<' '<<t.position.y<<")\">"<<t.text<<"</text>\n";
    return o.str()+"</svg>\n";
}
