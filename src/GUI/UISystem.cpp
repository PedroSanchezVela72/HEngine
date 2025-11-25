
#include "imgui.h"

#include "UISystem.h"
#include "RenderSystem.h"
#include "ImguiManager.h"
#include <Ogre.h>
#include "Manager.h"
#include "RenderMesh.h"
#include "Transform.h"
#include "UIElement.h"
#include "UIImageComponent.h"
#include "ButtonComponent.h"
#include "UITextComponent.h"
#include "UISliderComponent.h"
#include "InputField.h"
#include "Canvas.h"
#include <LoadLua.h>
#include <cstdint> 
#include <imgui_impl_opengl2.h>
#include <imgui_impl_sdl2.h>
#include "Debuglog.h"
#include "string"

using namespace H;

UISystem::UISystem(Manager* mngr, Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* renderWindow) : _mngr(mngr), _sceneMgr(sceneMgr), _renderWindow(renderWindow), numScene(0), fontsRemove(false){

    _ImguiMngr = Ogre::ImguiManager::getSingletonPtr();
#ifdef _DEBUG
    if(!_mngr->isRunningGame())
        addFontDefault();
#endif
    _showUIEditor = true;
  
}

UISystem::~UISystem() {
    _fontCache.clear();
}

void UISystem::update(double deltaTime) {
#ifdef _DEBUG
    if (_mngr->isRunningGame())
        _ImguiMngr->newFrame(deltaTime, Ogre::Rect(0, 0, _renderWindow->getWidth(), _renderWindow->getHeight()));
#else
    _ImguiMngr->newFrame(deltaTime, Ogre::Rect(0, 0, _renderWindow->getWidth(), _renderWindow->getHeight()));
#endif // !_DEBUG

    const std::vector<Entity*>& gameObjects = _mngr->getEntities(_grp_PARENTENTS);
    for (Entity* ent : gameObjects) {
        if (_mngr->hasComponent<Canvas>(ent)) {
#ifdef _DEBUG
            if(_showUIEditor || _mngr->isRunningGame())
#endif
                renderCanvas(deltaTime,ent);
        }
    }
    // Finalizar y renderizar ImGui
    ImGui::EndFrame();
    ImGui::Render();
}

void H::UISystem::initSystem()
{
   /* Message* m = new Message;
    m->id = _m_ADD_FONT_TEXTURE;
    _mngr->send(m, true);*/
}

void UISystem::receive(const Message* m) {
    switch (m->id) {
    case _m_INIT_ENTITY:
        addEntity(m->entity.entityPtr, m->entity.entityName);
        break;
    case _m_SHOW_UI_EDITOR:
        _showUIEditor = m->show_UI_editor.show;
        break;
    case _m_INIT_IMGUIMANAGER:
        _ImguiMngr->init(_sceneMgr);
        break;
    case _m_REMOVE_FONT_TEXTURE:
        _ImguiMngr->destroyFontTexture();
        addFontDefault();
        break;
    case _m_ADD_FONT_TEXTURE:
        numScene += m->scene.incScene;
        _ImguiMngr->addFontTexture();
        fontsRemove = false;
        break;
    }
}

void UISystem::renderCanvas(double deltaTime, Entity* ent) {
    
    Canvas* canvas = _mngr->getComponent<Canvas>(ent);
    if (!canvas) return; 

   
#ifdef _DEBUG
    ImVec2 posCanvas;
    if (_mngr->isRunningGame()) {
        posCanvas = { 0, 0 };
    }
    else {
        posCanvas = H_CANVAS_EDITOR_POS;
    }
#else 
    ImVec2 posCanvas = { 0, 0 };
#endif
    ImVec2 sizeCanvas = { canvas->size.x, canvas->size.y };

    
    ImGui::SetNextWindowSize(sizeCanvas);
    ImGui::SetNextWindowPos(posCanvas);

#ifdef _DEBUG
    if (_mngr->isRunningGame()) {
        if (canvas->input) {
            ImGui::Begin(canvas->name.c_str(), nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
        }
        else {

            ImGui::Begin(canvas->name.c_str(), nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
        }
    }
    else {
        ImGui::Begin(canvas->name.c_str(), nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    }
#else
    if (canvas->input) {
        ImGui::Begin(canvas->name.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
    }
    else {

        ImGui::Begin(canvas->name.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs); 
    }
#endif    

   

    
    for (Entity* uiEnt : ent->getChildren()) {
        entityIteration(uiEnt, ent->isActive() && uiEnt->isActive());
    }

    ImGui::End(); 

#ifdef _DEBUG
    if (!_mngr->isRunningGame()) {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        ImU32 borderColor = IM_COL32(255, 0, 0, 255);
        float borderThickness = 2.0f;

        drawList->AddRect(posCanvas,
            ImVec2(posCanvas.x + sizeCanvas.x, posCanvas.y + sizeCanvas.y),
            borderColor,
            0.0f,
            0,
            borderThickness);
    }
#endif
}


void UISystem::entityIteration(Entity* entity, bool activeParent)
{
    //Comprobar funcionamiento hijos
    if (_mngr->hasComponent<UITextComponent>(entity)) {
        if (activeParent && entity->isActive()) {
            renderText(entity);
        }
    }
    if (_mngr->hasComponent<UIImageComponent>(entity)) {
        if (activeParent && entity->isActive()) {
            renderUIImage(entity);
        }
    }
    if (_mngr->hasComponent<ButtonComponent>(entity)) {
        if (activeParent && entity->isActive()) {
            renderButton(entity);
        }
    }
    if (_mngr->hasComponent<UISliderComponent>(entity)) {
        if (activeParent && entity->isActive()) {
            renderSlider(entity);
        }
    }
    if (_mngr->hasComponent<InputField>(entity)) {
        if (activeParent && entity->isActive()) {
            renderInputField(entity);
        }
    }
}

void UISystem::renderUIImage(Entity* ent) {
    Transform* trComp = _mngr->getComponent<Transform>(ent);
    UIImageComponent* imgComp = _mngr->getComponent<UIImageComponent>(ent);

    if (!imgComp || !trComp) return;

    // Cargar la textura si no esta cargada
    Ogre::TextureManager& texMgr = Ogre::TextureManager::getSingleton();
    if (!texMgr.resourceExists(imgComp->texture)) {
        try {
            texMgr.load(imgComp->texture, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }
        catch (const Ogre::Exception& e) {
            std::string error = "Error: No se pudo cargar la textura '" + imgComp->texture + "': " + e.getDescription();
            DebugLog::instance()->throwLog(error);
            return;
        }
    }

    Ogre::TexturePtr ogreTexture = texMgr.getByName(imgComp->texture);
    if (!ogreTexture) {
        std::string error;
        error = "Error: No se pudo cargar la textura: " + imgComp->texture;
        DebugLog::instance()->throwLog(error);
        return;
    }

    Ogre::ResourceHandle handle = ogreTexture->getHandle();
    imgComp->textureID = (ImTextureID)(intptr_t)(handle);

    Vector2F posImg = { trComp->position.x, trComp->position.y };
    Vector2F sizeImg = { imgComp->sizeWind.x * trComp->scale.x,imgComp->sizeWind.y * trComp->scale.y };

#ifdef _DEBUG
    if (!_mngr->isRunningGame()) {
        posImg = editorConvFactor(posImg);
        sizeImg = editorConvFactor(sizeImg);
    }
#endif

    if (!_mngr->hasComponent<ButtonComponent>(ent)) {
        ImGui::SetCursorPos(ImVec2(posImg.x, posImg.y));
        ImVec2 imSizeImg = ImVec2(sizeImg.x, sizeImg.y);
#ifdef _DEBUG
        if(!_mngr->isRunningGame())
            ImGui::BeginChild(imgComp->name.c_str(), imSizeImg, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
        else
            ImGui::BeginChild(imgComp->name.c_str(), imSizeImg, false, ImGuiWindowFlags_NoScrollbar);
#else
        ImGui::BeginChild(imgComp->name.c_str(), imSizeImg, false, ImGuiWindowFlags_NoScrollbar);
#endif
        ImGui::SetCursorPos({ 0,0 });
        ImVec4 imgColor = { imgComp->color.r, imgComp->color.g, imgComp->color.b, imgComp->color.a };

        if (imgComp->tiled) {
            float texWidth = static_cast<float>(ogreTexture->getWidth());
            float texHeight = static_cast<float>(ogreTexture->getHeight());
            ImVec2 uv1(sizeImg.x / texWidth, sizeImg.y / texHeight);
            ImGui::Image(imgComp->textureID, imSizeImg, ImVec2(0, 0), uv1, imgColor);
        }
        else {
            ImGui::Image(imgComp->textureID, imSizeImg, ImVec2(0, 0), ImVec2(1, 1), imgColor);
        }
        std::list<Entity*> children = ent->getChildren();
        for (Entity* child : children)
        {
            entityIteration(child, ent->isActive() && child->isActive());
        }
        ImGui::EndChild();
    }
}




void UISystem::renderButton(Entity* ent) {
    Transform* trComp = _mngr->getComponent<Transform>(ent);
    ButtonComponent* buttonComp = _mngr->getComponent<ButtonComponent>(ent);

    Vector2F posButton = { trComp->position.x, trComp->position.y };
    Vector2F sizeButton = { buttonComp->sizeWind.x * trComp->scale.x, buttonComp->sizeWind.y * trComp->scale.y };
#ifdef _DEBUG
    if (!_mngr->isRunningGame()) {
        posButton = editorConvFactor(posButton);
        sizeButton = editorConvFactor(sizeButton);
    }
#endif

    // Renderizar el boton
    ImGui::SetCursorPos(ImVec2(posButton.x, posButton.y));
    ImVec2 imSizeButton = ImVec2(sizeButton.x, sizeButton.y);
#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        ImGui::BeginChild(buttonComp->name.c_str(), imSizeButton, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    else
        ImGui::BeginChild(buttonComp->name.c_str(), imSizeButton, false, ImGuiWindowFlags_NoScrollbar);
#else
    ImGui::BeginChild(buttonComp->name.c_str(), imSizeButton, false, ImGuiWindowFlags_NoScrollbar);
#endif
    ImGui::SetCursorPos({ 0,0 });
    if (_mngr->hasComponent<UIImageComponent>(ent)) {
        UIImageComponent* imgComp = _mngr->getComponent<UIImageComponent>(ent);
        ImVec4 imgColor = { imgComp->color.r, imgComp->color.g, imgComp->color.b, imgComp->color.a };
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        if (ImGui::ImageButton(buttonComp->label.c_str(), imgComp->textureID, imSizeButton, ImVec2(0, 0), ImVec2(1, 1),ImVec4(0,0,0,0), imgColor)) {
            if (buttonComp->interactive)
            {
                if (buttonComp->OnClick) {
                    buttonComp->OnClick();
                }
            }
        }
        // Si el widget esta en hover, dibujamos una superposicion para simular el efecto
        if (ImGui::IsItemHovered()) {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            // Define un color para el hover (ajusta la opacidad segun lo necesario)
            ImU32 hoverColor = ImGui::GetColorU32(ImVec4(imgColor.x, imgColor.y, imgColor.z, imgColor.w - 0.8f));
            draw_list->AddRectFilled(min, max, hoverColor);
        }
        ImGui::PopStyleVar(2);
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { buttonComp->colorHovered.r,buttonComp->colorHovered.g,buttonComp->colorHovered.b,buttonComp->colorHovered.a });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { buttonComp->colorActive.r,buttonComp->colorActive.g,buttonComp->colorActive.b,buttonComp->colorActive.a });
        ImGui::PushStyleColor(ImGuiCol_Button, { buttonComp->color.r,buttonComp->color.g,buttonComp->color.b,buttonComp->color.a });
        if (ImGui::Button(buttonComp->label.c_str(), imSizeButton))
        {
            if (buttonComp->interactive)
            {
                if (buttonComp->OnClick) {
                    buttonComp->OnClick();
                }
            }
        }
        ImGui::PopStyleColor(3);
    }
    std::list<Entity*> children = ent->getChildren();
    for (Entity* child : children)
    {
        entityIteration(child, ent->isActive() && child->isActive());
    }
    ImGui::EndChild();
    
}

void UISystem::renderText(Entity* ent) {
    Transform* trComp = _mngr->getComponent<Transform>(ent);
    UITextComponent* textComp = _mngr->getComponent<UITextComponent>(ent);
    Vector2F posText = { trComp->position.x, trComp->position.y }; 
    Vector2F windowSize = { textComp->sizeWind.x * trComp->scale.x, textComp->sizeWind.y * trComp->scale.y };
#ifdef _DEBUG
    if (!_mngr->isRunningGame()) {
        posText = editorConvFactor(posText);
        windowSize = editorConvFactor(windowSize);
    }
#endif

    ImGui::SetCursorPos(ImVec2(posText.x, posText.y));
    ImVec2 imWinSize = ImVec2(windowSize.x, windowSize.y);
#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        ImGui::BeginChild(textComp->name.c_str(), imWinSize, true, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    else
        ImGui::BeginChild(textComp->name.c_str(), imWinSize, false, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
#else
    ImGui::BeginChild(textComp->name.c_str(), imWinSize, false, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
#endif
        if (textComp->font != _fontDefault) {
            ImGui::PushFont(textComp->font);
        }

        // Calcular el tamano del texto
        ImVec2 textSize = ImGui::CalcTextSize(textComp->text.c_str());

        // Calcular posicion centrada dentro de la ventana hija
        ImVec2 textPos = {
            (windowSize.x - textSize.x) * textComp->alignment.x, // Centrar en X
            (windowSize.y - textSize.y) * textComp->alignment.y,  // Centrar en Y
        };
        
        ImGui::SetCursorPos(textPos); // Establecer posicion alineada
        ImVec4 textColor = { textComp->color.r, textComp->color.g, textComp->color.b, textComp->color.a };
        ImGui::TextColored(textColor, "%s", textComp->text.c_str());
        

        if (textComp->font != _fontDefault) {
            ImGui::PopFont();
        }
        std::list<Entity*> children = ent->getChildren();
        for (Entity* child : children)
        {
            entityIteration(child, ent->isActive() && child->isActive());
        }
    ImGui::EndChild();
    ImGui::SameLine();
}

void UISystem::renderSlider(Entity* ent) {
    Transform* trComp = _mngr->getComponent<Transform>(ent);
    UISliderComponent* sliderComp = _mngr->getComponent<UISliderComponent>(ent);

    Vector2F posSlider = { trComp->position.x, trComp->position.y };
#ifdef _DEBUG
    if(!_mngr->isRunningGame())
        posSlider = editorConvFactor(posSlider); 
#endif

    // Calcular tam del texto del slider
    if (sliderComp->font != _fontDefault) {
        ImGui::PushFont(sliderComp->font);
    }
    ImVec2 textSize = ImGui::CalcTextSize(sliderComp->text.c_str());
    

    float framePadding = ImGui::GetStyle().FramePadding.y * 2; // Padding arriba/abajo
    float itemSpacing = ImGui::GetStyle().ItemSpacing.y; // Espaciado entre elementos

#ifdef _DEBUG
    if (!_mngr->isRunningGame()){
        framePadding = editorConvFactor(framePadding);// Padding arriba/abajo
        itemSpacing = editorConvFactor(itemSpacing); // Espaciado entre elementos
    }
#endif // _DEBUG

    // Calcular tam dinamico del slider
    Vector2F sizeSlider = {
        sliderComp->sizeWind.x * trComp->scale.x + textSize.x, // Ancho fijo
        (textSize.y + framePadding + itemSpacing + ImGui::GetFrameHeight()) * trComp->scale.y
    };
#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        sizeSlider = editorConvFactor(sizeSlider);
#endif
    // Renderizar el slider
    ImGui::SetCursorPos(ImVec2(posSlider.x, posSlider.y));
    ImVec2 imSizeSlider = ImVec2(sizeSlider.x, sizeSlider.y);
#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        ImGui::BeginChild(sliderComp->name.c_str(), imSizeSlider, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    else
        ImGui::BeginChild(sliderComp->name.c_str(), imSizeSlider, false, ImGuiWindowFlags_NoScrollbar);
#else
    ImGui::BeginChild(sliderComp->name.c_str(), imSizeSlider, false, ImGuiWindowFlags_NoScrollbar);
#endif

    ImGui::PushStyleColor(ImGuiCol_FrameBg, { sliderComp->barColor.r,sliderComp->barColor.g,sliderComp->barColor.b,sliderComp->barColor.a});
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { sliderComp->barColorHover.r,sliderComp->barColorHover.g,sliderComp->barColorHover.b,sliderComp->barColorHover.a});
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { sliderComp->barColorActive.r,sliderComp->barColorActive.g,sliderComp->barColorActive.b,sliderComp->barColorActive.a });

    ImGui::PushStyleColor(ImGuiCol_SliderGrab, { sliderComp->handleColor.r,sliderComp->handleColor.g,sliderComp->handleColor.b,sliderComp->handleColor.a  });
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, { sliderComp->handleColorActive.r,sliderComp->handleColorActive.g,sliderComp->handleColorActive.b,sliderComp->handleColorActive.a });

    if (sliderComp->wholeNumbers) {
        ImGui::SliderInt(sliderComp->text.c_str(), (int*)&sliderComp->value, (int)sliderComp->minValue, (int)sliderComp->maxValue);
    }
    else {
        ImGui::SliderFloat(sliderComp->text.c_str(), &sliderComp->value, sliderComp->minValue, sliderComp->maxValue);
    }
    ImGui::PopStyleColor(5);
    if (sliderComp->font != _fontDefault) {
        ImGui::PopFont();
    }
    std::list<Entity*> children = ent->getChildren();
    for (Entity* child : children)
    {
        entityIteration(child, ent->isActive() && child->isActive());
    }
    ImGui::EndChild();
    
}

/// <summary>
/// Definicion del callback para InputText, con el que podremos limpiar el contenido del campo de texto en cada click
/// </summary>
/// <param name="data">estructura que contiene información sobre el estado del campo de texto y permite modificar su contenido dinamicamente</param>
int InputTextCallback(ImGuiInputTextCallbackData* data) {
    // Se obtiene el puntero al componente InputField a traves de UserData
    InputField* field = static_cast<InputField*>(data->UserData);
    // Si se ha indicado que se hizo click, se limpia el buffer
    if (field->clicked) {
        data->DeleteChars(0, data->BufTextLen);
        field->clicked = false;   // Se resetea la bandera para que solo se limpie una vez
    }
    return 0;
}

void UISystem::renderInputField(Entity* ent) {
    Transform* trComp = _mngr->getComponent<Transform>(ent);
    InputField* fieldComp = _mngr->getComponent<InputField>(ent);

    Vector2F posField = { trComp->position.x, trComp->position.y };
    Vector2F windowSize = { fieldComp->sizeWind.x * trComp->scale.x, fieldComp->sizeWind.y * trComp->scale.y };
#ifdef _DEBUG
    if(!_mngr->isRunningGame())
        posField = editorConvFactor(posField); 
#endif

    ImGui::SetCursorPos(ImVec2(posField.x, posField.y));
    ImVec2 imWinSize = ImVec2(windowSize.x, windowSize.y);
#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        windowSize = editorConvFactor(windowSize);
#endif
    ImVec4 bgColor = { fieldComp->backgroundColor.r, fieldComp->backgroundColor.g, fieldComp->backgroundColor.b, fieldComp->backgroundColor.a };
    ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);

#ifdef _DEBUG
    if (!_mngr->isRunningGame())
        ImGui::BeginChild(fieldComp->name.c_str(), imWinSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    else
        ImGui::BeginChild(fieldComp->name.c_str(), imWinSize, false, ImGuiWindowFlags_NoScrollbar);
#else
    ImGui::BeginChild(fieldComp->name.c_str(), imWinSize, false, ImGuiWindowFlags_NoScrollbar);
#endif

    if (fieldComp->font != _fontDefault) {
        ImGui::PushFont(fieldComp->font);
    }

    ImVec4 textColor = { fieldComp->textColor.r, fieldComp->textColor.g, fieldComp->textColor.b, fieldComp->textColor.a };
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);

    // Calcular tam y pos del texto
    ImVec2 textSize = ImGui::CalcTextSize(fieldComp->text.c_str());
    ImVec2 textPos = {
        (windowSize.x - textSize.x) * fieldComp->alignment.x,
        (windowSize.y - textSize.y) * fieldComp->alignment.y
    };
    ImGui::SetCursorPos(textPos);

    bool isEmpty = (fieldComp->buffer[0] == '\0');

    // Si el buffer esta vacio y no ha sido clickeado, mostramos el placeholder
    if (isEmpty && !fieldComp->clicked) {
        strncpy_s(fieldComp->buffer, sizeof(fieldComp->buffer), fieldComp->placeholder.c_str(), _TRUNCATE);
    }

    ImGui::SetNextItemWidth(-FLT_MIN);
    // Detectamos si el InputText fue clickeado
    if (ImGui::InputText("##InputField", fieldComp->buffer, H_INPUT_FIELD_MAX_LEN, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways,
        InputTextCallback, static_cast<void*>(fieldComp))) {
        fieldComp->text = fieldComp->buffer;  // Actualizamos el texto cuando se presiona Enter
        if (fieldComp->onSubmit) {
            fieldComp->onSubmit(fieldComp->text); // Llamamos a la funcion de submit
            fieldComp->clicked = false; // Reseteamos el estado de "clickeado"
        }
    }
    if (ImGui::IsItemClicked()) {
        memset(fieldComp->buffer, 0, sizeof(fieldComp->buffer));
        fieldComp->clicked = true;
    }


    if (fieldComp->font != _fontDefault) {
        ImGui::PopFont();
    }

    ImGui::PopStyleColor(2);

    std::list<Entity*> children = ent->getChildren();
    for (Entity* child : children) {
        entityIteration(child, ent->isActive() && child->isActive());
    }

    ImGui::EndChild();
}

std::string H::UISystem::deserializeText(const std::string& input)
{
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            if (input[i + 1] == 'n') {
                result += '\n';  // Convierte "\\n" en '\n'
                ++i;  // Saltar la 'n'
                continue;
            }
            else if (input[i + 1] == '"') {
                result += '"';  // Convierte \" en "
                ++i;  // Saltar la "
                continue;
            }
        }
        result += input[i];
    }
    return result;
}


void UISystem::addEntity(Entity* e, std::string name)
{
#ifndef _DEBUG
    if (numScene >= 1 && !fontsRemove) {
        _ImguiMngr->destroyFontTexture();
        _fontCache.clear();
        addFontDefault();
        fontsRemove = true;
    }
    else {
        if(_fontDefault == nullptr)
            addFontDefault();
    }
#else
    if (_mngr->isRunningGame()) {
        if (numScene >= 1 && !fontsRemove) {
            _ImguiMngr->destroyFontTexture();
            _fontCache.clear();
            addFontDefault();
            fontsRemove = true;
        }
        
    }
#endif

    LoadLua* lua = LoadLua::instance();

    if (lua->hasComponent(name, "Canvas")) {
#ifdef _DEBUG 
        Vector2F size;
        if (!_mngr->isRunningGame())
            size = Vector2F(H_CANVAS_EDITOR_WIDTH, H_CANVAS_EDITOR_HEIGHT);
        else
            size = Vector2F(H_WINDOW_WIDTH, H_WINDOW_HEIGHT);
#else
        Vector2F size = Vector2F(H_WINDOW_WIDTH, H_WINDOW_HEIGHT);
#endif
        bool resizable = lua->loadBoolValue(name, { "Canvas", "resizable" });
        bool movable = lua->loadBoolValue(name, { "Canvas", "movable" });
        bool input = false; 
        if (lua->hasComponent(name, { "Canvas", "input" }))
            input = lua->loadBoolValue(name, { "Canvas", "input" });

        Canvas* canvas = _mngr->addComponent<Canvas>(e, size, resizable, movable);
        canvas->name = name;
        canvas->input = input;
        _mngr->setHandler(_hdlr_UI, e);
    }
    if (lua->hasComponent(name, "ButtonComponent")) {
        bool interactive = lua->loadBoolValue(name, { "ButtonComponent", "interactive" });
        std::string label = lua->loadStrValue(name, { "ButtonComponent", "label" });
        Color color = {
            lua->loadFloatValue(name, {"ButtonComponent", "color", "r"}),
            lua->loadFloatValue(name, {"ButtonComponent", "color", "g"}),
            lua->loadFloatValue(name, {"ButtonComponent", "color", "b"}),
            lua->loadFloatValue(name, {"ButtonComponent", "color", "a"}),

        };
        Color hover = {
            lua->loadFloatValue(name, {"ButtonComponent", "hover", "r"}),
            lua->loadFloatValue(name, {"ButtonComponent", "hover", "g"}),
            lua->loadFloatValue(name, {"ButtonComponent", "hover", "b"}),
            lua->loadFloatValue(name, {"ButtonComponent", "hover", "a"}),

        };
        Color colorActive = {
            lua->loadFloatValue(name, {"ButtonComponent", "colorActive", "r"}),
            lua->loadFloatValue(name, {"ButtonComponent", "colorActive", "g"}),
            lua->loadFloatValue(name, {"ButtonComponent", "colorActive", "b"}),
            lua->loadFloatValue(name, {"ButtonComponent", "colorActive", "a"}),

        };
        Vector2F sizeWindow = {
           lua->loadFloatValue(name, {"ButtonComponent", "size", "x"}),
           lua->loadFloatValue(name, {"ButtonComponent", "size", "y"})
        };

        ButtonComponent* button = _mngr->addComponent<ButtonComponent>(e,interactive,label, color, hover, colorActive, sizeWindow);
        button->name = name;
        _mngr->setHandler(_hdlr_UI, e);
    }
    if (lua->hasComponent(name, "UITextComponent")) {
        std::string text = deserializeText(lua->loadStrValue(name, { "UITextComponent", "text" }));
        Color color = {
            lua->loadFloatValue(name, {"UITextComponent", "color", "r"}),
            lua->loadFloatValue(name, {"UITextComponent", "color", "g"}),
            lua->loadFloatValue(name, {"UITextComponent", "color", "b"}),
            lua->loadFloatValue(name, {"UITextComponent", "color", "a"}),

        };
        float fontSize = lua->loadFloatValue(name, { "UITextComponent", "fontSize" });
#ifdef _DEBUG
        if (!_mngr->isRunningGame())
            fontSize = editorConvFactor(fontSize);
#endif
        std::string font = lua->loadStrValue(name, { "UITextComponent", "font" });
        ImFont* fontText = nullptr;
        std::string fontKey = (font.empty() ? H_UI_DEF_FONT : font) + std::to_string((int)fontSize);

        if (_fontCache.find(fontKey) == _fontCache.end()) {
            try {
                fontText = _ImguiMngr->addFont(font.empty() ? H_UI_DEF_FONT : font, fontSize);
                _fontCache[fontKey] = fontText;
            }
            catch (const Ogre::FileNotFoundException& e) {
                std::string error = "No se ha encontrado el archivo .ttf: " + e.getDescription();
                DebugLog::instance()->throwLog(error);
                return;
            }
            catch (const Ogre::Exception& e) {
                fontText = _fontDefault;
                std::string error = "Ogre exception while loading font.";
                DebugLog::instance()->throwLog(error);
                return;
            }
        }
        else {
            fontText = _fontCache[fontKey];
        }

        

        Vector2F alignment = {
            lua->loadFloatValue(name, {"UITextComponent", "alignment", "x"}),
            lua->loadFloatValue(name, {"UITextComponent", "alignment", "y"}),
        };
        Vector2F sizeWindow = {
           lua->loadFloatValue(name, {"UITextComponent", "size", "x"}),
           lua->loadFloatValue(name, {"UITextComponent", "size", "y"})
        };
        UITextComponent* textComp = _mngr->addComponent<UITextComponent>(e, text, color, fontText, alignment, sizeWindow);
        textComp->name = name;
        _mngr->setHandler(_hdlr_UI, e);
    }
    if (lua->hasComponent(name, "UIImageComponent")) {
        
        std::string texture = lua->loadStrValue(name, { "UIImageComponent", "texture" });;
        Color color = {
            lua->loadFloatValue(name, {"UIImageComponent", "color", "r"}),
            lua->loadFloatValue(name, {"UIImageComponent", "color", "g"}),
            lua->loadFloatValue(name, {"UIImageComponent", "color", "b"}),
            lua->loadFloatValue(name, {"UIImageComponent", "color", "a"}),

        };
        bool tiled = lua->loadBoolValue(name, { "UIImageComponent", "tiled" });
        Vector2F sizeWindow = {
           lua->loadFloatValue(name, {"UIImageComponent", "size", "x"}),
           lua->loadFloatValue(name, {"UIImageComponent", "size", "y"})
        };
        UIImageComponent* imageComp = _mngr->addComponent<UIImageComponent>(e, texture, sizeWindow, color, tiled);
        imageComp->name = name;
        _mngr->setHandler(_hdlr_UI, e);
    }
    if (lua->hasComponent(name, "UISliderComponent")) {
        bool interactive = lua->loadBoolValue(name, { "UISliderComponent", "interactive" });
        float minValue = lua->loadFloatValue(name, { "UISliderComponent", "minValue" });
        float maxValue = lua->loadFloatValue(name, { "UISliderComponent", "maxValue" });
        float value = lua->loadFloatValue(name, { "UISliderComponent", "value" });
        bool wholeNumbers = lua->loadBoolValue(name, { "UISliderComponent", "wholeNumbers" });
        Color barcolor = {
            lua->loadFloatValue(name, {"UISliderComponent", "barcolor", "r"}),
            lua->loadFloatValue(name, {"UISliderComponent", "barcolor", "g"}),
            lua->loadFloatValue(name, {"UISliderComponent", "barcolor", "b"}),
            lua->loadFloatValue(name, {"UISliderComponent", "barcolor", "a"}),
        };
        Color barColorHover = {
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorHover", "r"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorHover", "g"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorHover", "b"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorHover", "a"}),
        };
        Color barColorActive = {
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorActive", "r"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorActive", "g"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorActive", "b"}),
           lua->loadFloatValue(name, {"UISliderComponent", "barcolorActive", "a"}),
        };
        Color handleColor = {
            lua->loadFloatValue(name, {"UISliderComponent", "handleColor", "r"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColor", "g"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColor", "b"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColor", "a"}),
        };
        Color handleColorActive = {
            lua->loadFloatValue(name, {"UISliderComponent", "handleColorActive", "r"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColorActive", "g"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColorActive", "b"}),
            lua->loadFloatValue(name, {"UISliderComponent", "handleColorActive", "a"}),
        };
        std::string text = lua->loadStrValue(name, { "UISliderComponent", "text" });
        Vector2F sizeWindow = {
           lua->loadFloatValue(name, {"UISliderComponent", "size", "x"}),
           lua->loadFloatValue(name, {"UISliderComponent", "size", "y"})
        };
        bool vertical = lua->loadBoolValue(name, { "UISliderComponent", "vertical" });
        float fontSize = lua->loadFloatValue(name, { "UISliderComponent", "fontSize" });
#ifdef _DEBUG
        if (!_mngr->isRunningGame())
            fontSize = editorConvFactor(fontSize);
#endif
        std::string font = lua->loadStrValue(name, { "UISliderComponent", "font" });
        ImFont* fontText = nullptr;
        std::string fontKey = (font.empty() ? H_UI_DEF_FONT : font) + std::to_string((int)fontSize);

        if (_fontCache.find(fontKey) == _fontCache.end()) {
            try {
                fontText = _ImguiMngr->addFont(font.empty() ? H_UI_DEF_FONT : font, fontSize);
                _fontCache[fontKey] = fontText;
            }
            catch (const Ogre::FileNotFoundException& e) {
                std::string error = "No se ha encontrado el archivo .ttf: " + e.getDescription();
                DebugLog::instance()->throwLog(error);
                return;
            }
            catch (const Ogre::Exception& e) {
                std::string error = "Ogre exception while loading font.";
                DebugLog::instance()->throwLog(error);
                fontText = _fontDefault;
                return;
            }
        }
        else {
            fontText = _fontCache[fontKey];
        }
        UISliderComponent* SliderComp = _mngr->addComponent<UISliderComponent>(e,interactive, minValue, maxValue, value, wholeNumbers, sizeWindow,
            barcolor, barColorHover, barColorActive, handleColor, handleColorActive, text, vertical, fontText);
        SliderComp->name = name;
        _mngr->setHandler(_hdlr_UI, e);
    }
    if (lua->hasComponent(name, "InputField")) {
        bool interactive = lua->loadBoolValue(name, { "InputField", "interactive" });
        std::string text = lua->loadStrValue(name, { "InputField", "text" });
        std::string placeholder = lua->loadStrValue(name, { "InputField", "placeholder" });
        Color textColor = {
            lua->loadFloatValue(name, {"InputField", "textColor", "r"}),
            lua->loadFloatValue(name, {"InputField", "textColor", "g"}),
            lua->loadFloatValue(name, {"InputField", "textColor", "b"}),
            lua->loadFloatValue(name, {"InputField", "textColor", "a"}),

        };
        Color backgroundColor = {
            lua->loadFloatValue(name, {"InputField", "backgroundColor", "r"}),
            lua->loadFloatValue(name, {"InputField", "backgroundColor", "g"}),
            lua->loadFloatValue(name, {"InputField", "backgroundColor", "b"}),
            lua->loadFloatValue(name, {"InputField", "backgroundColor", "a"}),

        };
        //ImFont
        std::string font = lua->loadStrValue(name, { "InputField", "font" });
        float fontSize = lua->loadFloatValue(name, { "InputField", "fontSize" });
#ifdef _DEBUG
        if (!_mngr->isRunningGame())
            fontSize = editorConvFactor(fontSize);
#endif
        ImFont* fontText = nullptr;
        std::string fontKey = (font.empty() ? H_UI_DEF_FONT : font) + std::to_string((int)fontSize);

        if (_fontCache.find(fontKey) == _fontCache.end()) {
            try {
                fontText = _ImguiMngr->addFont(font.empty() ? H_UI_DEF_FONT : font, fontSize);
                _fontCache[fontKey] = fontText;
            }
            catch (const Ogre::FileNotFoundException& e) {
                std::string error = "No se ha encontrado el archivo .ttf: " + e.getDescription();
                DebugLog::instance()->throwLog(error);
                return;
            }
            catch (const Ogre::Exception& e) {
                std::string error = "Ogre exception while loading font.";
                DebugLog::instance()->throwLog(error);
                fontText = _fontDefault;
                return;
            }
        }
        else {
            fontText = _fontCache[fontKey];
        }


        Vector2F alignment = {
            lua->loadFloatValue(name, {"InputField", "alignment", "x"}),
            lua->loadFloatValue(name, {"InputField", "alignment", "y"}),
        };
        Vector2F sizeWindow = {
           lua->loadFloatValue(name, {"InputField", "size", "x"}),
           lua->loadFloatValue(name, {"InputField", "size", "y"})
        };
        InputField* inputField = _mngr->addComponent<InputField>(e,interactive, text, placeholder, textColor, 
            backgroundColor, fontText, alignment, sizeWindow);
        inputField->name = name;
        _mngr->setHandler(_hdlr_UI, e);
    }
}
void H::UISystem::addFontDefault()
{
    std::string defaultKey = H_UI_DEF_FONT + std::to_string(H_UI_DEF_TXT_SIZE);
    try {
        _fontDefault = _ImguiMngr->addFont(H_UI_DEF_FONT, H_UI_DEF_TXT_SIZE);
        _fontCache[defaultKey] = _fontDefault;
    }
    catch (const Ogre::FileNotFoundException& e) {
        std::string error = "No se ha encontrado el archivo .ttf: " + e.getDescription();
        DebugLog::instance()->throwLog(error);
        return;
    }
    catch (const Ogre::Exception& e) {
        std::string error = "Ogre exception while loading font: ";
        DebugLog::instance()->throwLog(error);
        return;
    }
}
#ifdef _DEBUG
template<typename T>
T UISystem::editorConvFactor(const T& value)
{
    float factor = (float)(H_CANVAS_EDITOR_WIDTH) / (float)H_WINDOW_WIDTH;
    T convertedValue = value * factor;
    return convertedValue;
}
#endif