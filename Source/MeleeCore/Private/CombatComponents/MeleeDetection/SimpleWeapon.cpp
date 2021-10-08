#include "MeleeDetection/SimpleWeapon.h"

void USimpleWeapon::BeginPlay()
{
	Super::BeginPlay();

	m_Info.SocketNames.Empty();
	m_Info.SocketNames.Append(GetAllSocketNames());
}
