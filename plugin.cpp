#include "logger.h"

using namespace SKSE;
using namespace RE;
using namespace std;

////////////////////////////////////////////////////////////
// Will be added to library

/// <summary>
/// Executes a function for each equipped armor on an Actor.
/// </summary>
/// <typeparam name="FN"></typeparam>
/// <param name="a">Actor to check.</param>
/// <param name="DoSomething">Function of type <c>(TESObjectARMO* armor) =>
/// void</c></param>
template <typename FN>
void ForEachEquippedArmor(Actor* a, const FN& DoSomething) {
    if (!a) return;
    unsigned int slotFx = 1 << 31;
    for (unsigned int i = 1; i < slotFx; i *= 2) {
        auto* x = a->GetWornArmor(
            static_cast<BGSBipedObjectForm::BipedObjectSlot>(i));
        if (x) DoSomething(x);
    }
}

/// <summary>
/// Executes a function for each spell in a list.
/// </summary>
/// <typeparam name="FN">(SpellItem* spell) => void</typeparam>
/// <param name="spells">Spell list.</param>
/// <param name="DoSomething">Function with signature <c>(SpellItem* spell) => void.</c></param>
template <typename FN>
void ForEachSpell(TESSpellList::SpellData* spells, const FN& DoSomething) {
    for (size_t i{0}; i < spells->numSpells; i++) {
        auto spell = spells->spells[i];
        if (spell) DoSomething(spell);
    }
}

////////////////////////////////////////////////////////////
void TestActor(Actor* a) {
    if (!a) {
        log::error("No actor");
        return;
    }

    log::info("Checking spells");
    auto* spells = a->GetActorBase()->GetSpellList();
    ForEachSpell(spells, [](SpellItem* spell) {
        log::info("Spell lambda: {}", spell->GetName());
    });

    log::info("Checking body armor");
    auto armr = a->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kBody);
    if (armr)
        log::info("Body armor: {}", armr->GetName());
    else
        log::info("No body armor");

    log::info("Checking equipped armors");
    ForEachEquippedArmor(a, [](TESObjectARMO* armor) { 
        log::info("Equipped armor: {}", armor->GetName()); 
    });
}

void OnGameLoaded() {
    log::info("Game was loaded. Applying appearance to nearby NPCs");
    TestActor(TESForm::LookupByID<Actor>(0x14));
}

void OnDataLoaded() {
    auto* sweetroll = TESForm::LookupByID(0x64b3d);
    log::debug("Name: {} Value: {}", 
               sweetroll->GetName(),
               sweetroll->GetGoldValue());
    TestActor(TESForm::LookupByID<Actor>(0x14));
}

void OnMessage(MessagingInterface::Message* msg) {
    auto mt{msg->type};

    if (mt == MessagingInterface::kNewGame)
        log::info("New game. Configuring Max Sick Gains for first use.");
    else if (mt == MessagingInterface::kPostLoadGame)
        OnGameLoaded();
    else if (mt == MessagingInterface::kDataLoaded)
        OnDataLoaded();
}

SKSEPluginLoad(const LoadInterface* skse) {
    Init(skse);

    SetupLog();
    log::debug("Max Sick Gains is ready to work");

    GetMessagingInterface()->RegisterListener(OnMessage);

    return true;
}
