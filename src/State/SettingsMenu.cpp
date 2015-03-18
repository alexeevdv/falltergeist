/*
 * Copyright 2012-2015 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// C++ standard includes

// Falltergeist includes
#include "../functions.h"
#include "../Game/Game.h"
#include "../Graphics/Renderer.h"
#include "../ResourceManager.h"
#include "../State/SettingsMenu.h"
#include "../UI/Image.h"
#include "../UI/ImageButton.h"
#include "../UI/MultistateImageButton.h"
#include "../UI/Slider.h"
#include "../UI/TextArea.h"
#include "../Settings.h"
#include "../Input/Mouse.h"

// Third party includes

namespace Falltergeist
{
namespace State
{

SettingsMenu::SettingsMenu() : State()
{
}

SettingsMenu::~SettingsMenu()
{
}

void SettingsMenu::init()
{
    if (_initialized) return;
    State::init();

    setModal(true);
    setFullscreen(true);

    // background
    auto background = new Image("art/intrface/prefscrn.frm");
    auto backgroundX = (Game::getInstance()->renderer()->width() - background->width())*0.5;
    auto backgroundY = (Game::getInstance()->renderer()->height() - background->height())*0.5;
    background->setX(backgroundX);
    background->setY(backgroundY);
    addUI(background);

    // Switches (big)
    auto combatDifficultySwitch = new MultistateImageButton(MultistateImageButton::TYPE_BIG_SWITCH, backgroundX+76, backgroundY+71);
    combatDifficultySwitch->setMaxState(3);
    combatDifficultySwitch->setState(Game::getInstance()->settings()->combatDifficulty());
    addUI("combat_difficulty",combatDifficultySwitch);

    auto gameDifficultySwitch = new MultistateImageButton(MultistateImageButton::TYPE_BIG_SWITCH, backgroundX+76, backgroundY+149);
    gameDifficultySwitch->setMaxState(3);
    gameDifficultySwitch->setState(Game::getInstance()->settings()->gameDifficulty());
    addUI("game_difficulty",gameDifficultySwitch);

    auto violenceLevelSwitch = new MultistateImageButton(MultistateImageButton::TYPE_BIG_SWITCH, backgroundX+76, backgroundY+227);
    violenceLevelSwitch->setState(Game::getInstance()->settings()->violenceLevel());
    addUI("violence_level",violenceLevelSwitch);

    auto targetHighlightSwitch = new MultistateImageButton(MultistateImageButton::TYPE_BIG_SWITCH, backgroundX+76, backgroundY+309);
    targetHighlightSwitch->setMaxState(3);
    targetHighlightSwitch->setState(Game::getInstance()->settings()->targetHighlight());
    addUI("target_highlight",targetHighlightSwitch);

    auto combatLooksSwitch = new MultistateImageButton(MultistateImageButton::TYPE_BIG_SWITCH, backgroundX+76, backgroundY+387);
    combatLooksSwitch->setMaxState(2);
    combatLooksSwitch->setState(Game::getInstance()->settings()->combatLooks());
    addUI("combat_looks",combatLooksSwitch);

    // Switches (small)
    auto combatMessagesSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74);
    combatMessagesSwitch->setState(Game::getInstance()->settings()->combatMessages());
    addUI("combat_messages",combatMessagesSwitch);

    auto combatTauntsSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74+66);
    combatTauntsSwitch->setState(Game::getInstance()->settings()->combatTaunts());
    addUI("combat_taunts",combatTauntsSwitch);

    auto languageFilterSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74+66*2);
    languageFilterSwitch->setState(Game::getInstance()->settings()->languageFilter());
    addUI("language_filter",languageFilterSwitch);

    auto runningSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74+66*3);
    runningSwitch->setState(Game::getInstance()->settings()->running());
    addUI("running",runningSwitch);

    auto subtitlesSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74+66*4);
    subtitlesSwitch->setState(Game::getInstance()->settings()->subtitles());
    addUI("subtitles",subtitlesSwitch);

    auto itemHightlightSwitch = new MultistateImageButton(MultistateImageButton::TYPE_SMALL_SWITCH, backgroundX+299, backgroundY+74+66*5);
    itemHightlightSwitch->setState(Game::getInstance()->settings()->itemHighlight());
    addUI("item_highlight",itemHightlightSwitch);

    // LABELS
    auto font1_907824ff = ResourceManager::font("font1.aaf", 0x907824ff);
    auto font3_907824ff = ResourceManager::font("font3.aaf", 0x907824ff);
    auto font4_907824ff = ResourceManager::font("font4.aaf", 0x907824ff);

    // GAME PREFERENCES
    _addTextArea(_t(MSG_OPTIONS, 100), backgroundX+74, backgroundY+10)->setFont(font4_907824ff);

    // COMBAT DIFFICULTY
    auto difficulty = _addTextArea(_t(MSG_OPTIONS, 102), backgroundX+21, backgroundY+48);
    difficulty->setWidth(158)->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_CENTER)->setFont(font1_907824ff);

    // GAME DIFFICULTY
    _addTextArea(difficulty, backgroundX+21, backgroundY+48+77)->setText(_t(MSG_OPTIONS, 101));

    // VIOLENCE LEVEL
    _addTextArea(difficulty, backgroundX+21, backgroundY+48+156)->setText(_t(MSG_OPTIONS, 103));

    // TARGET HIGHLIGHT
    _addTextArea(difficulty, backgroundX+21, backgroundY+128+158)->setText(_t(MSG_OPTIONS, 104));

    // COMBAT LOOKS
    _addTextArea(difficulty, backgroundX+21, backgroundY+128+235)->setText(_t(MSG_OPTIONS, 105));

    // COMBAT MESSAGES
    auto combatMessages = _addTextArea(_t(MSG_OPTIONS, 106), backgroundX+206, backgroundY+49);
    combatMessages->setFont(font1_907824ff);

    // COMBAT TAUNTS
    _addTextArea(combatMessages, backgroundX+206, backgroundY+49+66)->setText(_t(MSG_OPTIONS, 107));

    // LANGUAGE FILTER
    _addTextArea(combatMessages, backgroundX+206, backgroundY+49+66*2)->setText(_t(MSG_OPTIONS, 108));

    // RUNNING
    _addTextArea(combatMessages, backgroundX+206, backgroundY+49+66*3)->setText(_t(MSG_OPTIONS, 109));

    // SUBTITLES
    _addTextArea(combatMessages, backgroundX+206, backgroundY+49+66*4)->setText(_t(MSG_OPTIONS, 110));

    // ITEM HIGHLIGHT
    _addTextArea(combatMessages, backgroundX+206, backgroundY+49+66*5)->setText(_t(MSG_OPTIONS, 111));

    // COMBAT SPEED
    auto combatSpeed = _addTextArea(_t(MSG_OPTIONS, 112), backgroundX+384, backgroundY+19);
    combatSpeed->setFont(font1_907824ff);

    // TEXT DELAY
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+95)->setText(_t(MSG_OPTIONS, 113));

    // MASTER AUDIO VOLUME
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165)->setText(_t(MSG_OPTIONS, 114));

    // MUSIC/MOVIE VOLUME
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165+51)->setText(_t(MSG_OPTIONS, 115));

    // SOUND EFFECTS VOLUME
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165+51*2)->setText(_t(MSG_OPTIONS, 116));

    // SPEECH VOLUME
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165+51*3)->setText(_t(MSG_OPTIONS, 117));

    // BRIGHTNESS LEVEL
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165+51*4)->setText(_t(MSG_OPTIONS, 118));

    // MOUSE SENSITIVITY
    _addTextArea(combatSpeed, backgroundX+384, backgroundY+165+51*5)->setText(_t(MSG_OPTIONS, 119));

    // DEFAULT BUTTON LABEL
    _addTextArea(combatSpeed, backgroundX+43, backgroundY+449)->setText(_t(MSG_OPTIONS, 120))->setFont(font3_907824ff);

    // DONE BUTTON LABEL
    _addTextArea(combatSpeed, backgroundX+169, backgroundY+449)->setText(_t(MSG_OPTIONS, 300))->setFont(font3_907824ff);

    // CANCEL BUTTON LABEL
    _addTextArea(combatSpeed, backgroundX+283, backgroundY+449)->setText(_t(MSG_OPTIONS, 121))->setFont(font3_907824ff);

    // COMBAT DIFFICULTY SWITCH LABELS
    _addTextArea(_t(MSG_OPTIONS, 203), backgroundX+43, backgroundY+81)->setFont(font1_907824ff);     // EASY
    _addTextArea(_t(MSG_OPTIONS, 204), backgroundX+68, backgroundY+67)->setFont(font1_907824ff);     // NORMAL
    _addTextArea(_t(MSG_OPTIONS, 205), backgroundX+122, backgroundY+81)->setFont(font1_907824ff);    // HARD

    // GAME DIFFICULTY SWITCH LABELS
    _addTextArea(_t(MSG_OPTIONS, 206), backgroundX+13, backgroundY+159)->setFont(font1_907824ff);    // WIMPY
    _addTextArea(_t(MSG_OPTIONS, 207), backgroundX+68, backgroundY+145)->setFont(font1_907824ff);    // NORMAL
    _addTextArea(_t(MSG_OPTIONS, 208), backgroundX+122, backgroundY+159)->setFont(font1_907824ff);   // ROUGH

    // VIOLENCE LEVEL SWITCH LABELS
    _addTextArea(_t(MSG_OPTIONS, 214), backgroundX+56, backgroundY+236)->setFont(font1_907824ff);    // NONE
    _addTextArea(_t(MSG_OPTIONS, 215), backgroundX+65, backgroundY+222)->setFont(font1_907824ff);    // MINIMUM
    _addTextArea(_t(MSG_OPTIONS, 207), backgroundX+122, backgroundY+236)->setFont(font1_907824ff);   // NORMAL
    _addTextArea(_t(MSG_OPTIONS, 216), backgroundX+122, backgroundY+257)->setFont(font1_907824ff)->setWidth(60); // MAXIMUM BLOOD

    // TARGET HIGHLIGHT SWITCH LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+42, backgroundY+319)->setFont(font1_907824ff);    // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+88, backgroundY+305)->setFont(font1_907824ff);    // ON
    _addTextArea(_t(MSG_OPTIONS, 213), backgroundX+122, backgroundY+319)->setFont(font1_907824ff)->setWidth(70)->setWordWrap(true); // TARGETING ONLY

    // COMBAT LOOKS SWITCH LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+42, backgroundY+397)->setFont(font1_907824ff);    // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+88, backgroundY+383)->setFont(font1_907824ff);    // ON

    // COMBAT MESSAGES SWITCH LABELS
    auto verboseLabel = _addTextArea(_t(MSG_OPTIONS, 211), backgroundX+203, backgroundY+69);         // VERBOSE
    verboseLabel->setFont(font1_907824ff)
                ->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_RIGHT)
                ->setWidth(100);
    _addTextArea(_t(MSG_OPTIONS, 212), backgroundX+320, backgroundY+69)->setFont(font1_907824ff);    // BRIEF

    // COMBAT TAUNTS SWITCH LABELS
    _addTextArea(verboseLabel, backgroundX+203, backgroundY+69+67)->setText(_t(MSG_OPTIONS, 202)); // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+320, backgroundY+69+67)->setFont(font1_907824ff); // ON

    // LANGUAGE FILTER SWITCH LABELS
    _addTextArea(verboseLabel, backgroundX+203, backgroundY+69+67+66)->setText(_t(MSG_OPTIONS, 202)); // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+320, backgroundY+69+67+66)->setFont(font1_907824ff); // ON

    // RUNNING SWITCH LABELS
    _addTextArea(verboseLabel, backgroundX+203, backgroundY+69+67+66+64)->setText(_t(MSG_OPTIONS, 209)); // NORMAL
    _addTextArea(_t(MSG_OPTIONS, 219), backgroundX+320, backgroundY+69+67+66+64)->setFont(font1_907824ff); // ALWAYS

    // SUBTITLES SWITCH LABELS
    _addTextArea(verboseLabel, backgroundX+203, backgroundY+69+67+66+66+65)->setText(_t(MSG_OPTIONS, 202)); // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+320, backgroundY+69+66+67+66+65)->setFont(font1_907824ff); // OFF

    // ITEM HIGHLIGHT SWITCH LABELS
    _addTextArea(verboseLabel, backgroundX+203, backgroundY+69+67+66+64+65+68)->setText(_t(MSG_OPTIONS, 202)); // OFF
    _addTextArea(_t(MSG_OPTIONS, 201), backgroundX+320, backgroundY+69+64+67+66+65+68)->setFont(font1_907824ff); // ON

    // AFFECT PLAYER SPEECH
    _addTextArea(_t(MSG_OPTIONS, 122), backgroundX+405, backgroundY+72)->setFont(font1_907824ff);

    // COMBAT SPEED SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+384, backgroundY+38)->setFont(font1_907824ff);      // NORMAL
    auto fastestLabel = _addTextArea(_t(MSG_OPTIONS, 210), backgroundX+524, backgroundY+38);           // FASTEST
    fastestLabel->setFont(font1_907824ff)
            ->setHorizontalAlign(TextArea::HORIZONTAL_ALIGN_RIGHT)
            ->setWidth(100);

    // TEXT DELAY SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 217), backgroundX+384, backgroundY+113)->setFont(font1_907824ff);     // SLOW
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+469, backgroundY+113)->setFont(font1_907824ff);     // NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+113)->setText(_t(MSG_OPTIONS, 218));       // FASTER

    // MASTER AUDIO VOLUME SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+384, backgroundY+184)->setFont(font1_907824ff);     // OFF
    _addTextArea(_t(MSG_OPTIONS, 221), backgroundX+450, backgroundY+184)->setFont(font1_907824ff);     // QUIET
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+494, backgroundY+184)->setFont(font1_907824ff);     // NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184)->setText(_t(MSG_OPTIONS, 222));       // LOUD

    // MUSIC/MOVIE VOLUME SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+384, backgroundY+184+51)->setFont(font1_907824ff);  // OFF
    _addTextArea(_t(MSG_OPTIONS, 221), backgroundX+450, backgroundY+184+51)->setFont(font1_907824ff);  // QUIET
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+494, backgroundY+184+51)->setFont(font1_907824ff);  // NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184+51)->setText(_t(MSG_OPTIONS, 222));    // LOUD

    // SOUND EFFECTS SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+384, backgroundY+184+51*2)->setFont(font1_907824ff);// OFF
    _addTextArea(_t(MSG_OPTIONS, 221), backgroundX+450, backgroundY+184+51*2)->setFont(font1_907824ff);// QUIET
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+494, backgroundY+184+51*2)->setFont(font1_907824ff);// NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184+51*2)->setText(_t(MSG_OPTIONS, 222));  // LOUD

    // SPEECH VOLUME SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 202), backgroundX+384, backgroundY+184+51*3)->setFont(font1_907824ff);// OFF
    _addTextArea(_t(MSG_OPTIONS, 221), backgroundX+450, backgroundY+184+51*3)->setFont(font1_907824ff);// QUIET
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+494, backgroundY+184+51*3)->setFont(font1_907824ff);// NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184+51*3)->setText(_t(MSG_OPTIONS, 222));  // LOUD

    // BRIGHTNESS LEVEL SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+384, backgroundY+184+51*4)->setFont(font1_907824ff);// NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184+51*4)->setText(_t(MSG_OPTIONS, 223));  // BRIGHTER

    // MOUSE SENSITIVITY SLIDER LABELS
    _addTextArea(_t(MSG_OPTIONS, 209), backgroundX+384, backgroundY+184+51*5)->setFont(font1_907824ff);// NORMAL
    _addTextArea(fastestLabel, backgroundX+524, backgroundY+184+51*5)->setText(_t(MSG_OPTIONS, 218));  // FASTER

    // BUTTONS

    // button: Default
    auto defaultButton = new ImageButton(ImageButton::TYPE_SMALL_RED_CIRCLE, backgroundX+23, backgroundY+450);
    defaultButton->addEventHandler("mouseleftclick", [this](Event* event){ this->onDefaultButtonClick(dynamic_cast<MouseEvent*>(event)); });
    addUI(defaultButton);

    // button: Done
    auto doneButton = new ImageButton(ImageButton::TYPE_SMALL_RED_CIRCLE, backgroundX+148, backgroundY+450);
    doneButton->addEventHandler("mouseleftclick", [this](Event* event){ this->doSave(); });
    addUI(doneButton);

    // button: Cancel
    auto cancelButton = new ImageButton(ImageButton::TYPE_SMALL_RED_CIRCLE, backgroundX+263, backgroundY+450);
    cancelButton->addEventHandler("mouseleftclick", [this](Event* event){ this->doCancel(); });
    addUI(cancelButton);

    // button: Affect player speed
    auto affectPlayerSpeedCheckBox = new ImageButton(ImageButton::TYPE_CHECKBOX, backgroundX+383, backgroundY+68);
    affectPlayerSpeedCheckBox->setChecked(Game::getInstance()->settings()->playerSpeedup());
    addUI("player_speedup", affectPlayerSpeedCheckBox);

    // SLIDERS
    // COMBAT SPEED SLIDER
    auto combatSpeedSlider = new Slider(backgroundX+384, backgroundY+50);
    combatSpeedSlider->setMinValue(0.0);
    combatSpeedSlider->setMaxValue(50.0);
    combatSpeedSlider->setValue(Game::getInstance()->settings()->combatSpeed());
    addUI("combat_speed",combatSpeedSlider);

    // TEXT DELAY SLIDER
    auto textDelaySlider = new Slider(backgroundX+384, backgroundY+125);
    textDelaySlider->setValue(Game::getInstance()->settings()->textDelay());
    addUI("text_delay",textDelaySlider);

    // MASTER AUDIO VOLUME SLIDER
    auto masterAudioVolumeSlider = new Slider(backgroundX+384, backgroundY+196);
    masterAudioVolumeSlider->setValue(Game::getInstance()->settings()->masterVolume());
    addUI("master_volume", masterAudioVolumeSlider);

    // MUSIC VOLUME SLIDER
    auto musicVolumeSlider = new Slider(backgroundX+384, backgroundY+196+51);
    musicVolumeSlider->setValue(Game::getInstance()->settings()->musicVolume());
    addUI("music_volume", musicVolumeSlider);

    // SOUND EFFECTS VOLUME SLIDER
    auto soundEffectsVolumeSlider = new Slider(backgroundX+384, backgroundY+196+51*2);
    soundEffectsVolumeSlider->setValue(Game::getInstance()->settings()->sfxVolume());
    addUI("sfx_volume", soundEffectsVolumeSlider);

    // SPEECH VOLUME SLIDER
    auto speechVolumeSlider = new Slider(backgroundX+384, backgroundY+196+51*3);
    speechVolumeSlider->setValue(Game::getInstance()->settings()->voiceVolume());
    addUI("voice_volume", speechVolumeSlider);

    // BRIGHTNESS LEVEL SLIDER
    auto brightnessLevelSlider = new Slider(backgroundX+384, backgroundY+196+51*4);
    brightnessLevelSlider->setValue(Game::getInstance()->settings()->brightness());
    addUI("brightness", brightnessLevelSlider);

    // MOUSE SENSITIVITY SLIDER
    auto mouseSensitivitySlider = new Slider(backgroundX+384, backgroundY+196+51*5);
    mouseSensitivitySlider->setValue(Game::getInstance()->settings()->mouseSensitivity());
    addUI("mouse_sensitivity",mouseSensitivitySlider);
}

void SettingsMenu::think()
{
    State::think();
}

//IniFileSection SettingsMenu::_getSettings()
//{
//    // @todo Load options from FALLTERGEIST_DATA/config.ini if exists
//
//    // or return default values
//    return _getDefaultSettings();
//}
//
//IniFileSection SettingsMenu::_getDefaultSettings()
//{
//    return IniFileSection({
//        {       "brightness", "1.000000"}, // max 1.179993
//        {"combat_difficulty", "1"},
//        {     "combat_looks", "0"},
//        {  "combat_messages", "1"},
//        {     "combat_speed", "0"},
//        {    "combat_taunts", "1"},
//        {  "game_difficulty", "1"},
//        {   "item_highlight", "1"},
//        {  "language_filter", "0"},
//        {"mouse_sensitivity", "1.000000"},  // max 2.500000
//        {   "player_speedup", "0"},
//        {          "running", "0"},
//        {        "subtitles", "0"},
//        { "target_highlight", "2"},
//        {  "text_base_delay", "3.500000"},  // max 1.000000
//        {  "text_line_delay", "1.000000"},  // max 0.000000
//        {   "violence_level", "3"}
//    });
//}

TextArea* SettingsMenu::_addLabel(std::string name, TextArea* label)
{
    _labels.insert(std::make_pair(name, label));
    return label;
}

TextArea* SettingsMenu::_addTextArea(std::string message, unsigned int x, unsigned int y)
{
    auto textArea = new TextArea(message, x, y);
    addUI(textArea);
    return textArea;
}

TextArea* SettingsMenu::_addTextArea(TextArea* parent, unsigned int x, unsigned int y)
{
    auto textArea = new TextArea(parent, x, y);
    addUI(textArea);
    return textArea;
}

void SettingsMenu::doCancel()
{
    // TODO: restore volume and mouse sensitivity
    Game::getInstance()->popState();
}

void SettingsMenu::doSave()
{
    Game::getInstance()->settings()->setCombatDifficulty(((MultistateImageButton*)getUI("combat_difficulty"))->state());
    Game::getInstance()->settings()->setGameDifficulty(((MultistateImageButton*)getUI("game_difficulty"))->state());
    Game::getInstance()->settings()->setViolenceLevel(((MultistateImageButton*)getUI("violence_level"))->state());
    Game::getInstance()->settings()->setTargetHighlight(((MultistateImageButton*)getUI("target_highlight"))->state());
    Game::getInstance()->settings()->setCombatLooks(((MultistateImageButton*)getUI("combat_looks"))->state());
    Game::getInstance()->settings()->setCombatMessages(((MultistateImageButton*)getUI("combat_messages"))->state());
    Game::getInstance()->settings()->setCombatTaunts(((MultistateImageButton*)getUI("combat_taunts"))->state());
    Game::getInstance()->settings()->setLanguageFilter(((MultistateImageButton*)getUI("language_filter"))->state());
    Game::getInstance()->settings()->setRunning(((MultistateImageButton*)getUI("running"))->state());
    Game::getInstance()->settings()->setSubtitles(((MultistateImageButton*)getUI("subtitles"))->state());
    Game::getInstance()->settings()->setItemHighlight(((MultistateImageButton*)getUI("item_highlight"))->state());

    Game::getInstance()->settings()->setMasterVolume(((Slider*)getUI("master_volume"))->value());
    Game::getInstance()->settings()->setMusicVolume(((Slider*)getUI("music_volume"))->value());
    Game::getInstance()->settings()->setVoiceVolume(((Slider*)getUI("voice_volume"))->value());
    Game::getInstance()->settings()->setSfxVolume(((Slider*)getUI("sfx_volume"))->value());

    Game::getInstance()->settings()->setTextDelay(((Slider*)getUI("text_delay"))->value());
    Game::getInstance()->settings()->setCombatSpeed(((Slider*)getUI("combat_speed"))->value());
    Game::getInstance()->settings()->setBrightness(((Slider*)getUI("brightness"))->value());
    Game::getInstance()->settings()->setMouseSensitivity(((Slider*)getUI("mouse_sensitivity"))->value());

    Game::getInstance()->settings()->setPlayerSpeedup(((ImageButton*)getUI("player_speedup"))->checked());

    Game::getInstance()->settings()->saveConfig();
    Game::getInstance()->popState();
}

void SettingsMenu::onDefaultButtonClick(MouseEvent* event)
{
}

void SettingsMenu::onKeyDown(KeyboardEvent* event)
{
    switch (event->keyCode())
    {
        case SDLK_ESCAPE:
            doCancel();
            break;
        case SDLK_RETURN:
            doSave();
            break;
    }
}

void SettingsMenu::onStateActivate(StateEvent* event)
{
    Game::getInstance()->mouse()->pushState(Mouse::BIG_ARROW);
}

void SettingsMenu::onStateDeactivate(StateEvent* event)
{
    Game::getInstance()->mouse()->popState();
}


}
}
