#pragma once
#include <stdint.h>
#include <functional>
#include "logger.hpp"

namespace Anim {

    class Events {
    public:
        static Events* GetSingleton() {
            static Events singleton;
            return &singleton;
        }

        static RE::BSEventNotifyControl ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);

        static inline REL::Relocation<decltype(ProcessEvent)> _PCProcessEvent;

        static RE::BSEventNotifyControl PCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src);

        static void AnimationEvent(const char* holder, const char* name);
        void AddEventSink();

    protected:
        Events() = default;
        Events(const Events&) = delete;
        Events(Events&&) = delete;
        virtual ~Events() = default;

        auto operator=(const Events&) -> Events& = delete;
        auto operator=(Events&&) -> Events& = delete;
    };

    RE::BSEventNotifyControl Anim::Events::PCProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src) {
        ProcessEvent(a_this, a_event, a_src);
        return _PCProcessEvent(a_this, a_event, a_src);
    }

    RE::BSEventNotifyControl Anim::Events::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_src) {
        if (a_event.tag != NULL && a_event.holder != NULL) {
            if (a_event.holder->IsPlayerRef())
                AnimationEvent("Player", a_event.tag.c_str());
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    void Anim::Events::AddEventSink() {
        REL::Relocation<uintptr_t> PCProcessAnimGraphEventVtbl{RE::VTABLE_PlayerCharacter[2]};
        _PCProcessEvent = PCProcessAnimGraphEventVtbl.write_vfunc(0x1, &PCProcessEvent);
    }
}

void InitializeAnimationHooking(){
    logger::debug("Initializing trampoline...");
    auto& trampoline = SKSE::GetTrampoline();
    trampoline.create(14);
    logger::debug("Trampoline initialized");
    Anim::Events::GetSingleton()->AddEventSink();
    const auto a_events = Anim::Events::GetSingleton();
    ///a_events->enabled = true;
    logger::info("Registered for player animation events");
}

#pragma region How to use

/*
Call InitializeAnimationHooking and then employ AnimationEvent where you want to process captured animation events
*/

// Executes when animation events occur
void Anim::Events::AnimationEvent(const char* holder, const char* a_event) {
    std::string eventName(a_event); // Convert passed-in argument to string
    logger::trace("{}: {}", holder, eventName);  // Output all the captured events to the log
    if (eventName == "weaponDraw" || eventName == "weaponSheathe") {  // Check if triggering animation event was drawing or sheathing weapon, bow, spells, etc. 
        logger::debug("weapon has been drawn or sheathed!"); 
        // do other stuff
    }
}


#pragma endregion

#pragma region Source Credit

// Animation event tracking functionality modified from "Animation Event Logger" by hsoju (MIT License)
// Links ==> https://github.com/hsoju/anim-event-logger; https://www.nexusmods.com/skyrimspecialedition/mods/82296

#pragma endregion