// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved


#include "AIOptimizerTypes.h"
#include "AIOInvokerComponent.h"
#include "AIOSubjectComponent.h"





FAIOSubject::FAIOSubject( UAIOSubjectComponent* InComponent ) :
	Component( InComponent )
{
	Priority = Component->Priority;
}

bool FAIOSubject::IsOwnerValid()
{
	return IsValid( Component );
}

FVector FAIOSubject::GetActorLocation() const
{
	return Component->GetOwner()->GetActorLocation();
}

AActor* FAIOSubject::GetActor() const
{
	return Component->GetOwner();
}





FORCEINLINE bool FAIOSubject::operator==( const FAIOSubjectHandle& OtherHandle ) const
{
	return IsValid( Component ) ? Component->GetHandle() == OtherHandle : false;
}

bool FAIOInvoker::IsOwnerValid()
{
	return IsValid( Component );
}

FVector FAIOInvoker::GetActorLocation() const
{
	return Component->GetOwner()->GetActorLocation();
}

AActor* FAIOInvoker::GetActor() const
{
	return Component->GetOwner();
}
