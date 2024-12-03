#include "BleConnectionWidget.h"
#include "CybSDK_Plugin.h"

void UBleConnectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ObjectsHolders.Add(SelectingObjectsHolder);
    ObjectsHolders.Add(ConnectionObjectsHolder);
    ObjectsHolders.Add(FailedObjectsHolder);
    ObjectsHolders.Add(CalibrationObjectsHolder);

    if (NativeConstructDelegate.IsBound())
    {
        NativeConstructDelegate.ExecuteIfBound();
    }
}