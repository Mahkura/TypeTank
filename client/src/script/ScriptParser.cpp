#include <script/ScriptParser.hpp>
#include <cstdint>
#include <array>

namespace tt
{
struct Token
{
    enum Type : uint16_t
    {
        Unknown,
        Unit,
        Move,
        Identifier,
        ScopeStart,
        ScopeEnd,
        Eof,
    };
    Type t;
    std::string content;
};

struct TokenTypeString
{
    Token::Type type;
    std::string_view text;
};

std::array<TokenTypeString, 4> s_token_strings =
{
    TokenTypeString{Token::ScopeStart, "{"},
    {Token::ScopeEnd, "}"},
    {Token::Unit, "unit"},
    {Token::Move, "move"},
};

static auto read_next_token(std::string_view& inout__script, Token& out__token) -> bool
{
    auto it_char = std::find_if(
        inout__script.begin(),
        inout__script.end(),
        [&](auto c){ return 0 == std::isspace(c); });

    if (it_char == inout__script.end())
    {
        inout__script = {};
        return false;
    }

    auto it_ws = std::find_if(
        it_char + 1,
        inout__script.end(),
        [&](auto c){ return 0 != std::isspace(c); });
    
    out__token = {};
    auto token_str = std::string_view(it_char, it_ws);
    auto it_token_s_t = std::find_if(
        s_token_strings.begin(),
        s_token_strings.end(),
        [&](auto& ts){ return ts.text == token_str; });
    if (it_token_s_t != s_token_strings.end())
        out__token.t = it_token_s_t->type;
    else
    {
        out__token.t = Token::Identifier;
        out__token.content = token_str;
    } 

    inout__script = inout__script.substr(std::distance(
        inout__script.begin(),
        it_ws
    ));

    return true;
}

template<typename T>
auto parse_number(std::string_view cand, T& out__r) -> char const*
{
    if (cand.empty() || std::isdigit(cand[0]) == 0)
        return nullptr;
    
    out__r = cand[0] - '0';
    if (out__r == 0) // do not allow leading zeroes
        return nullptr;

    cand = cand.substr(1);
    while (!cand.empty() && std::isdigit(cand[0]) != 0)
    {
        out__r *= 10;
        out__r += cand[0] - '0';
        cand = cand.substr(1);
    }

    return cand.data();
}

void ScriptParser::register_input(EventSink& sink)
{
    sink.register_queue<ScriptChanged>(&input_queue);
}

void ScriptParser::execute()
{
    if (input_queue.empty())
        return;

    auto script_content = std::shared_ptr<std::string>{};
    std::visit(
        [&](auto &&e) {
            using T = std::decay_t<decltype(e)>;
            if constexpr(std::is_same_v<T, ScriptChanged>)
                script_content = std::move(e.script_content);
        },
        input_queue.front());
    
    input_queue.pop();
 
    auto script = std::string_view{*script_content};
    auto token = Token{};
    while (read_next_token(/*inout*/script, /*out*/token))
    {
        if (Token::Unit == token.t)
        {
            auto event = UnitId{};
            if (read_next_token(/*inout*/script, /*out*/token))
            {
                if (Token::Identifier != token.t)
                    break;

                event.name = token.content;
                event.unit_id = 0u;
                output_sink->put_event(event);

                if (read_next_token(/*inout*/ script, /*out*/ token))
                {
                    if (Token::ScopeStart != token.t)
                        break;
                }
                
                if (read_next_token(/*inout*/ script, /*out*/ token))
                {
                    if (Token::Move == token.t)
                    {
                        auto e = UnitMove{};
                        if (read_next_token(/*inout*/ script, /*out*/ token))
                        {
                            if (Token::Identifier != token.t)
                                break;
                            
                            e.type = UnitMove::Meters;
                            parse_number(token.content, /*out*/e.move_amount);
                            if (e.move_amount <= 600)
                                output_sink->put_event(e);
                        }
                    }
                }
            }
        }
    }
}

} // namespace tt
