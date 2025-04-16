QT += core gui widgets svg

TARGET = CircuitsSimulator
TEMPLATE = app

CONFIG += c++20

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

SOURCES += \
    Grid.cpp \
    Window.cpp \
    Scene.cpp \
    commands/AddVariablesCommand.cpp \
    commands/ChangeVariablesCommand.cpp \
    commands/EmbedParamCommand.cpp \
    commands/InsertObjectsCommand.cpp \
    commands/MergeSelectionCommand.cpp \
    commands/MovePointsCommand.cpp \
    commands/RemoveObjectsCommand.cpp \
    commands/RemoveVariablesCommand.cpp \
    commands/SplitDipoleCommand.cpp \
    commands/Timeline.cpp \
    objects/BCPath.cpp \
    objects/Dipole.cpp \
    tools/MouseTool.cpp \
    tools/Selector.cpp \
    tools/Grabber.cpp \
    tools/Pen.cpp \
    utils/Geometry.cpp \
    utils/Globals.cpp \
    utils/Selection.cpp \
    main.cpp \
    widgets/MainPanel.cpp \
    widgets/custom/CustomDoubleSpinBox.cpp \
    widgets/custom/CustomSpinBox.cpp \
    widgets/groups/AliasGroup.cpp \
    widgets/groups/ColorsGroup.cpp \
    widgets/groups/DipoleGroup.cpp \
    widgets/groups/InputGroup.cpp \
    widgets/groups/ObjectGroup.cpp \
    widgets/groups/PenGroup.cpp \
    widgets/inputs/ColorInput.cpp \
    widgets/inputs/DoubleParameterInput.cpp \
    widgets/tabs/QuickSettingsTab.cpp \
    widgets/tabs/SelectionTab.cpp \
    widgets/tabs/VariablesTab.cpp \
    dialogs/PreferencesDialog.cpp

HEADERS += \
    Grid.h \
    Scene.h \
    Window.h \
    commands/AddVariablesCommand.h \
    commands/ChangeVariablesCommand.h \
    commands/Command.h \
    commands/EmbedParamCommand.h \
    commands/InsertObjectsCommand.h \
    commands/MergeSelectionCommand.h \
    commands/MovePointsCommand.h \
    commands/RemoveObjectsCommand.h \
    commands/RemoveVariablesCommand.h \
    commands/SplitDipoleCommand.h \
    commands/Timeline.h \
    objects/Alias.h \
    objects/BCControlPoint.h \
    objects/BCPath.h \
    objects/BCPoint.h \
    objects/Battery.h \
    objects/Capacitor.h \
    objects/DCI.h \
    objects/DCV.h \
    objects/Dipole.h \
    objects/Inductor.h \
    objects/Object.h \
    objects/Resistor.h \
    objects/WorldPoint.h \
    tools/MouseTool.h \
    tools/Grabber.h \
    tools/Pen.h \
    tools/Selector.h \
    utils/Geometry.h \
    utils/Selection.h \
    utils/Globals.h \
    widgets/MainPanel.h \
    widgets/custom/CustomDoubleSpinBox.h \
    widgets/custom/CustomSpinBox.h \
    widgets/groups/AliasGroup.h \
    widgets/groups/ColorsGroup.h \
    widgets/groups/DipoleGroup.h \
    widgets/groups/InputGroup.h \
    widgets/groups/ObjectGroup.h \
    widgets/groups/PenGroup.h \
    widgets/inputs/AddressInput.h \
    widgets/inputs/BooleanInput.h \
    widgets/inputs/ColorInput.h \
    widgets/inputs/CoordinateInput.h \
    widgets/inputs/DoubleParameterInput.h \
    widgets/inputs/IDLabel.h \
    widgets/inputs/RadiusInput.h \
    widgets/tabs/QuickSettingsTab.h \
    widgets/tabs/SelectionTab.h \
    widgets/tabs/VariablesTab.h \
    dialogs/PreferencesDialog.h

# Deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += assets.qrc
