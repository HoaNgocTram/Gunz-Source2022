void Super()
{
	ZCharacter* pChar = ZGetGameInterface()->GetMyCharacter();
	rvector dir = -RealSpace2::RCameraDirection;
	//ZGetEffectManager()->AddDynamiteEffect((D3DXVECTOR3)pChar->GetPosition(), dir);
	ZGetEffectManager()->AddTrapFireEffect((D3DXVECTOR3)pChar->GetPosition(), dir);
	ZGetSoundEngine()->PlaySound("fx_ki", pChar->GetPosition());
}