#include "attack.hpp"
#include "bitboard.hpp"
#include "position.hpp"
#include "raycast.hpp"

namespace chess {

[[nodiscard]] bool attacked(const Position &pos, const int sq, const bool them) {
    const auto bb = Bitboard(sq);
    const auto kt = pos.colour[them] & pos.pieces[static_cast<int>(Piece::Knight)];
    const auto BQ = pos.pieces[static_cast<int>(Piece::Bishop)] | pos.pieces[static_cast<int>(Piece::Queen)];
    const auto RQ = pos.pieces[static_cast<int>(Piece::Rook)] | pos.pieces[static_cast<int>(Piece::Queen)];
    const auto pawns = pos.colour[them] & pos.pieces[static_cast<int>(Piece::Pawn)];
    const auto pawn_attacks = them ? pawns.sw() | pawns.se() : pawns.nw() | pawns.ne();

    return
        // Pawns
        (pawn_attacks & bb) |
        // Knights
        (bb & kt.knight()) |
        // Bishops - Queens
        (raycast::bishop(sq, pos.colour[0] | pos.colour[1]) & pos.colour[them] & BQ) |
        // Rooks - Queens
        (raycast::rook(sq, pos.colour[0] | pos.colour[1]) & pos.colour[them] & RQ) |
        // King
        (bb.adjacent() & pos.colour[them] & pos.pieces[static_cast<int>(Piece::King)]);
}

}  // namespace chess
