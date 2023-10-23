#include <script/ScriptParser.hpp>

#define BOOST_UT_DISABLE_MODULE
#include <ut.hpp>

using namespace ::boost::ut;

struct Fixture
{
    tt::EventQueue output_queue;
    tt::EventSink sink;
    tt::ScriptParser sut;

    Fixture()
    {
        sut.output_sink = &sink;
        sink.register_queue<tt::UnitId, tt::UnitMove>(
            &output_queue);
    }

    void execute()
    {
        sut.execute();
    }

    void set_script_content(std::string script)
    {
        sut.script_content = script;
    }
};

int main(int argc, char** argv)
{
    "unit name is reported"_test = []
    {
        auto test = Fixture{};
        test.set_script_content(R"(
            unit TankXYZ {
                move 600m
            }
        )");

        test.execute();

        expect((test.output_queue.size() == 2) >> fatal);
        auto e = std::get<tt::UnitId>(test.output_queue.front());
        expect(e.name == "TankXYZ");
        expect(e.unit_id == 0u);
        test.output_queue.pop();
        auto em = std::get<tt::UnitMove>(test.output_queue.front());
        expect(em.unit_id == 0u);
        expect(em.move_amount == 600u);
        expect(em.type == tt::UnitMove::Meters);
    };

    // TODO: test that 'move' by itself does not work (unless it should? perhaps select random unit?)
    // TODO: too big numbers
    // TODO: move amount without 'm'
    // TODO: move amount with unsupported trailing character
    // TODO: verify that multiple calls to execute without setting new script only execute script once
    return 0;
}