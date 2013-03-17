/*
	adc_reset() resets averages
	call periodically adc_update() to sum-up readings
	call adc_snap() to compute the averages. Now you can use the adc.ave1... from the datastructure
*/
struct _adc
{
	WORD raw1;
	WORD raw2;
	WORD raw3;

	BYTE count;

	WORD ave1;		// average readings 0...3FF
	WORD ave2;
	WORD ave3;

	float BT;		// power supply voltage in Volts
	float PS;		// battery voltage in Volts
	float Ic;		// total current in Amps
	float Th;

} adc;

void adc_reset()
{
	adc.count = 0;
	adc.raw1 = 0;
	adc.raw2 = 0;
	adc.raw3 = 0;

}
void adc_update()
{
	if( adc.count >=64 )					// avoid overflows
		return;
	adc.raw1 += read_V1();
	adc.raw2 += read_V2();
	adc.raw3 += read_Ic();
	adc.count++;
}
void adc_compute()
{
	if( adc.count == 0 )
		adc.count = 1;						// should never happen
	adc.ave1 = adc.raw1/adc.count;
	adc.ave2 = adc.raw2/adc.count;
	adc.ave3 = adc.raw3/adc.count;

	adc.BT = calc_V1( adc.ave1 );
	adc.PS = calc_V2( adc.ave2 );
	adc.Ic = calc_Ic( adc.ave3 );
//	adc.Th = (float)(reading.wTh);
}