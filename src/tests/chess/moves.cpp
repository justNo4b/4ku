#include <catch2/catch.hpp>
#include <chess/attack.hpp>
#include <chess/fen.hpp>
#include <chess/makemove.hpp>
#include <chess/move.hpp>
#include <chess/movegen.hpp>
#include <chess/position.hpp>
#include <chess/undomove.hpp>
#include <string>

void valid(const chess::Position &pos, const chess::Move &move) {
    const auto piece = chess::piece_on(pos, move.from);
    const auto captured = chess::piece_on(pos, move.to);

    REQUIRE(piece == move.piece);

    switch (move.type) {
        case chess::Move::Type::Quiet:
            break;
        case chess::Move::Type::Double:
            REQUIRE((24 <= move.to && move.to <= 31));
            REQUIRE((8 <= move.from && move.from <= 15));
            REQUIRE(move.piece == chess::Piece::Pawn);
            break;
        case chess::Move::Type::Capture:
            REQUIRE(captured == move.captured);
            REQUIRE(captured != chess::Piece::King);
            break;
        case chess::Move::Type::Enpassant:
            REQUIRE((40 <= move.to && move.to <= 47));
            REQUIRE((32 <= move.from && move.from <= 39));
            break;
        case chess::Move::Type::Ksc:
            REQUIRE(move.piece == chess::Piece::King);
            REQUIRE(move.to == 4);
            REQUIRE(move.to == 6);
            break;
        case chess::Move::Type::Qsc:
            REQUIRE(move.piece == chess::Piece::King);
            REQUIRE(move.to == 4);
            REQUIRE(move.to == 2);
            break;
        case chess::Move::Type::Promo:
            REQUIRE((56 <= move.to && move.to <= 63));
            REQUIRE((48 <= move.from && move.from <= 55));
            REQUIRE(move.piece == chess::Piece::Pawn);
            REQUIRE(move.promo != chess::Piece::Pawn);
            REQUIRE(move.promo != chess::Piece::King);
            break;
        case chess::Move::Type::Promocapture:
            REQUIRE((56 <= move.to && move.to <= 63));
            REQUIRE((48 <= move.from && move.from <= 55));
            REQUIRE(move.piece == chess::Piece::Pawn);
            REQUIRE(move.promo != chess::Piece::Pawn);
            REQUIRE(move.promo != chess::Piece::King);
            REQUIRE(captured == move.captured);
            REQUIRE(captured != chess::Piece::King);
            break;
        default:
            FAIL();
            break;
    }
}

void test(chess::Position &pos, const int depth) {
    if (depth == 0) {
        return;
    }

    chess::Move moves[256];
    const int num_moves = chess::movegen(pos, moves);

    for (int i = 0; i < num_moves; ++i) {
        valid(pos, moves[i]);

        chess::makemove(pos, moves[i]);

        const int ksq = (pos.colour[1] & pos.pieces[static_cast<int>(chess::Piece::King)]).lsbll();

        if (chess::attacked(pos, ksq, false)) {
            chess::undomove(pos, moves[i]);
            continue;
        }

        test(pos, depth - 1);

        chess::undomove(pos, moves[i]);
    }
}

TEST_CASE("Move validity") {
    const std::string fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0",
        "2rq1rk1/pp1bppbp/2np1np1/8/3NP3/1BN1BP2/PPPQ2PP/2KR3R b - - 8",
        "2rqr1k1/pp1bppbp/3p1np1/4n3/3NP2P/1BN1BP2/PPPQ2P1/1K1R3R b - - 0",
        "rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0",
        "4k3/2b3q1/3P1P2/4K3/3P1P2/2b3q1/8/8 w - - 0 1",
        "k7/8/4r3/3pP3/8/8/8/4K3 w - d6 0 1",
    };

    chess::Position pos;
    chess::Move moves[256];

    for (const auto &fen : fens) {
        INFO(fen);
        set_fen(pos, fen);
        test(pos, 3);
    }
}
