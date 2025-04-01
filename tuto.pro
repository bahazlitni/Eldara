QT += core gui
QT += svgwidgets

TARGET = CircuitsSimulator
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++20

SOURCES += \
    Grid.cpp \
    VariablesManager.cpp \
    Window.cpp \
    App.cpp \
    commands/AddVariablesCommand.cpp \
    commands/ChangeAttributeCommand.cpp \
    commands/ChangeVariablesCommand.cpp \
    commands/EntangleValueToObjectsCommand.cpp \
    commands/InsertObjectsCommand.cpp \
    commands/MergeSelectionCommand.cpp \
    commands/MovePointsCommand.cpp \
    commands/RemoveVariablesCommand.cpp \
    commands/SplitDipoleCommand.cpp \
    objects/BCPath.cpp \
    tools/MouseTool.cpp \
    tools/Selector.cpp \
    utils/Functions.cpp \
    commands/Timeline.cpp \
    objects/Dipole.cpp \
    objects/Wire.cpp \
    utils/Geometry.cpp \
    tools/Grabber.cpp \
    tools/Pen.cpp \
    main.cpp \
    utils/Selection.cpp \
    widgets/custom/CustomDoubleSpinBox.cpp \
    widgets/custom/CustomSpinBox.cpp \
    widgets/groups/ColorsGroup.cpp \
    widgets/groups/DipoleGroup.cpp \
    widgets/groups/PenGroup.cpp \
    widgets/groups/UnitDipoleGroup.cpp \
    widgets/inputs/ColorInput.cpp \
    widgets/inputs/DoubleParameterInput.cpp \
    widgets/tabs/QuickSettingsTab.cpp \
    widgets/tabs/SelectionTab.cpp \
    widgets/tabs/VariablesTab.cpp \
    widgets\MainPanel.cpp \
    widgets\groups\AliasGroup.cpp \
    widgets\groups\InputGroup.cpp \
    widgets\groups\ObjectGroup.cpp \
    commands/RemoveObjectsCommand.cpp \



HEADERS += \
    Grid.h \
    App.h \
    VariablesManager.h \
    commands/AddVariablesCommand.h \
    commands/ChangeAttributeCommand.h \
    commands/ChangeVariablesCommand.h \
    commands/Command.h \
    commands/EntangleValueToObjectsCommand.h \
    commands/InsertObjectsCommand.h \
    commands/MergeSelectionCommand.h \
    commands/MovePointsCommand.h \
    commands/RemoveVariablesCommand.h \
    commands/SplitDipoleCommand.h \
    objects/BCControlPoint.h \
    objects/BCPath.h \
    objects/BCPoint.h \
    objects/Object.h \
    objects/Resistor.h \
    objects/Inductor.h \
    commands/Timeline.h \
    commands/RemoveObjectsCommand.h \
    objects/Alias.h \
    objects/Battery.h \
    objects/Capacitor.h \
    objects/DC_CurrentGenerator.h \
    objects/DC_VoltageGenerator.h \
    objects/Dipole.h \
    objects/UnitDipole.h \
    objects/WorldPoint.h \
    tools/MouseTool.h \
    utils/Functions.h \
    utils/Geometry.h \
    tools/Grabber.h \
    tools/Pen.h \
    tools/Selector.h \
    Window.h \
    objects/Wire.h \
    utils/Selection.h \
    utils/Types.h \
    widgets/custom/CustomDoubleSpinBox.h \
    widgets/custom/CustomSpinBox.h \
    widgets/groups/ColorsGroup.h \
    widgets/groups/PenGroup.h \
    widgets/groups/UnitDipoleGroup.h \
    widgets/inputs/AddressInput.h \
    widgets/inputs/BooleanInput.h \
    widgets/inputs/DoubleParameterInput.h \
    widgets/inputs/ColorInput.h \
    widgets/inputs/CoordinateInput.h \
    widgets/HSeparator.h \
    widgets/inputs/IDLabel.h \
    widgets/inputs/ObjectTypeInput.h \
    widgets/inputs/RadiusInput.h \
    widgets/groups/DipoleGroup.h \
    widgets/tabs/QuickSettingsTab.h \
    widgets/tabs/SelectionTab.h \
    widgets/tabs/VariablesTab.h \
    widgets\MainPanel.h \
    widgets\groups\AliasGroup.h \
    widgets\groups\InputGroup.h \
    widgets\groups\ObjectGroup.h \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets.qrc
