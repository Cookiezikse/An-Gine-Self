#include "UDP/Test/AckHandler_Test.h"

void AckHandler_Test::Test()
{
	static constexpr uint64_t MASK_COMPLETE = std::numeric_limits<uint64_t>::max();
	static constexpr uint64_t MASK_FIRST_ACKED = Bousk::Utils::Bit<uint64_t>::Right;
	static constexpr uint64_t MASK_FIRST_MISSING = ~MASK_FIRST_ACKED;
	static constexpr uint64_t MASK_LAST_ACKED = (MASK_FIRST_ACKED << 63);
	Bousk::UDP::AckHandler ackhandler;

	CHECK(ackhandler.lastAck() == std::numeric_limits<uint16_t>::max());
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	CHECK(!ackhandler.isAcked(0));
	CHECK(!ackhandler.isNewlyAcked(0));
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #0
	ackhandler.update(0, MASK_COMPLETE, true);
	CHECK(ackhandler.lastAck() == 0);
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	CHECK(ackhandler.isAcked(0));
	CHECK(ackhandler.isNewlyAcked(0));
	CHECK(ackhandler.getNewAcks().size() == 1);
	CHECK(ackhandler.getNewAcks()[0] == 0);
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #2 avec identifiant #1 manquant (63e bit du masque à 0) 
	ackhandler.update(2, MASK_FIRST_MISSING, true);
	CHECK(ackhandler.lastAck() == 2);
	CHECK(ackhandler.previousAcksMask() == MASK_FIRST_MISSING);
	CHECK(ackhandler.isAcked(2));
	CHECK(ackhandler.isAcked(0));
	CHECK(ackhandler.isNewlyAcked(2));
	CHECK(!ackhandler.isNewlyAcked(0));
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #1
	ackhandler.update(1, MASK_COMPLETE, true);
	CHECK(ackhandler.lastAck() == 2);
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	CHECK(ackhandler.isAcked(1));
	CHECK(ackhandler.isAcked(2));
	CHECK(ackhandler.isAcked(0));
	CHECK(ackhandler.isNewlyAcked(1));
	CHECK(!ackhandler.isNewlyAcked(2));
	CHECK(!ackhandler.isNewlyAcked(0));
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #66 avec masque vide : tous les identifiants [3,65] sont manquants, mais pas encore perdus
	ackhandler.update(66, 0, true);
	CHECK(ackhandler.lastAck() == 66);
	CHECK(ackhandler.isNewlyAcked(66));
	CHECK(ackhandler.previousAcksMask() == MASK_LAST_ACKED);
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #67 avec masque vide
	ackhandler.update(67, 0, true);
	CHECK(ackhandler.lastAck() == 67);
	CHECK(ackhandler.isNewlyAcked(67));
	CHECK(!ackhandler.isNewlyAcked(66));
	CHECK(ackhandler.previousAcksMask() == MASK_FIRST_ACKED);
	CHECK(ackhandler.loss().empty());

	//!< Réception de l’identifiant #68, avec masque complet, l’identifiant #3 est maintenant perdu
	ackhandler.update(68, MASK_COMPLETE, true);
	CHECK(ackhandler.lastAck() == 68);
	CHECK(ackhandler.isNewlyAcked(68));
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	{
		auto loss = ackhandler.loss();
		CHECK(loss.size() == 1);
		CHECK(loss[0] == 3);
	}
	for (uint16_t i = 4; i < 66; i++)
	{
		CHECK(ackhandler.isNewlyAcked(i));
	}

	//!< Réception d’un identifiant plus vieux que le masque
	ackhandler.update(0, 0, true);
	CHECK(ackhandler.lastAck() == 68);
	CHECK(!ackhandler.isNewlyAcked(68));
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);

	//!< Saut de 65 identifiants, réception de #133 avec masque vide
	ackhandler.update(133, 0, true);
	CHECK(ackhandler.lastAck() == 133);
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	CHECK(ackhandler.loss().empty());

	//!< Saut de 100 identifiants avec masque complet, identifiants [134, 169] sont perdus
	ackhandler.update(234, 0, true);
	CHECK(ackhandler.lastAck() == 234);
	CHECK(ackhandler.previousAcksMask() == 0);
	{
		auto loss = ackhandler.loss();
		const auto firstLost = 134;
		const auto lastLost = 169;
		const auto totalLost = lastLost - firstLost + 1;
		CHECK(loss.size() == totalLost);
		for (auto i = 0; i < totalLost; i++)
		{
			CHECK(loss[i] == firstLost + i);
		}
	}
	ackhandler.update(234, MASK_COMPLETE, true);
	ackhandler.update(236, MASK_COMPLETE, true);

	//!< Saut de 65 identifiants avec masque vide
	ackhandler.update(301, 0, true);
	CHECK(ackhandler.lastAck() == 301);
	CHECK(ackhandler.previousAcksMask() == 0);
	CHECK(ackhandler.loss().empty());
	CHECK(ackhandler.isAcked(237));

	//!< Acquittement de l’identifiant #237
	ackhandler.update(237, MASK_COMPLETE, true);
	CHECK(ackhandler.lastAck() == 301);
	CHECK(ackhandler.previousAcksMask() == MASK_LAST_ACKED);
	CHECK(ackhandler.loss().empty());
	CHECK(ackhandler.isAcked(237));
	CHECK(ackhandler.isNewlyAcked(237));

	//!< Acquittement de tous les identifiants via masque complet et doublon de #301
	ackhandler.update(301, MASK_COMPLETE, true);
	CHECK(ackhandler.lastAck() == 301);
	CHECK(ackhandler.previousAcksMask() == MASK_COMPLETE);
	CHECK(ackhandler.loss().empty());

	//!< Vérification de la transformation du masque en identifiants
	ackhandler.update(303, MASK_COMPLETE, true);
	auto newAcks = ackhandler.getNewAcks();
	CHECK(newAcks.size() == 2);
	CHECK(newAcks[0] == 302);
	CHECK(newAcks[1] == 303);
}