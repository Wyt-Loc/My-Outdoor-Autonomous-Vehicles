using System;
using System.Text;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace IMLibrary.AV
{
	/// <summary>
	/// UT ��ժҪ˵����
	/// </summary>
	public class UT
	{
		public UT()
		{
			//
			// TODO: �ڴ˴���ӹ��캯���߼�
			//
		}
		/////////////////////////////////////////////////////////////////////////

		// LowPassWave

		//

		// ��ͨ�˲�

		//

		// ������Format ���� ������Ƶ��ʽ�ṹWAVEFORMATEX

		//      lpData ���� ������Ƶ���ݿ�ָ��

		//      dwDataLength ���� ������Ƶ���ݿ��С

		//      fFrequencyPass ���� �˲�Ƶ����ֵ

		//

		// �޷���ֵ

		/////////////////////////////////////////////////////////////////////////

		unsafe 	public static void LowPassWave(WAVEFORMATEX Format, byte[] data,             

			int dwDataLength, float fFrequencyPass)

		{

			fixed(byte* lpData=data)
			{
				float fParam0,fParam1,fParam2;

				int   nSamplesPerSec = Format.nSamplesPerSec;

 

				fParam0=(1.0f/nSamplesPerSec)/(2.0f/fFrequencyPass+

					1.0f/nSamplesPerSec);

				fParam1=fParam0;

				fParam2=(1.0f/nSamplesPerSec-2.0f/fFrequencyPass)/

					(2.0f/fFrequencyPass+1.0f/nSamplesPerSec);

 

				PassWave(Format, lpData, dwDataLength, fFrequencyPass, 

					fParam0, fParam1, fParam2);
			}

		}

 

		/////////////////////////////////////////////////////////////////////////

		// HighPassWave

		//

		// ��ͨ�˲�

		//

		// ������Format ���� ������Ƶ��ʽ�ṹWAVEFORMATEX

		//      lpData ���� ������Ƶ���ݿ�ָ��

		//      dwDataLength ���� ������Ƶ���ݿ��С

		//      fFrequencyPass ���� �˲�Ƶ����ֵ

		//

		// �޷���ֵ

		/////////////////////////////////////////////////////////////////////////

		unsafe public static void HighPassWave(WAVEFORMATEX Format,byte[] data,

			int dwDataLength, float fFrequencyPass)

		{


			fixed(byte* lpData=data)
			{
				float fParam0,fParam1,fParam2;

				int   nSamplesPerSec = Format.nSamplesPerSec;

 

				fParam0=(20.0f/fFrequencyPass+1.0f/nSamplesPerSec)/

					(2.0f/fFrequencyPass+1.0f/nSamplesPerSec);

				fParam1=(-20.0f/fFrequencyPass+1.0f/nSamplesPerSec)/

					(2.0f/fFrequencyPass+1.0f/nSamplesPerSec);

				fParam2=(1.0f/nSamplesPerSec-2.0f/fFrequencyPass)/

					(2.0f/fFrequencyPass+1.0f/nSamplesPerSec);

 

				PassWave(Format, lpData, dwDataLength, fFrequencyPass, 

					fParam0, fParam1, fParam2);
			}

		}
		/////////////////////////////////////////////////////////////////////////

		// PassWave

		//

		// �˲��㷨

		//

		// ������Format ���� ������Ƶ��ʽ�ṹWAVEFORMATEX

		//      lpData ���� ������Ƶ���ݿ�ָ��

		//      dwDataLength ���� ������Ƶ���ݿ��С

		//      fFrequencyPass ���� �˲�Ƶ����ֵ

		//      fParam0, fParam1, fParam2 ���� �˲�����

		//

		// �޷���ֵ

		/////////////////////////////////////////////////////////////////////////

		unsafe private static void PassWave(WAVEFORMATEX Format, byte* lpData, 

			int dwDataLength, float fFrequencyPass,

			float fParam0,float fParam1, float fParam2)

		{

			float fXL0,fXL1,fYL1,fYL0;

			float fXR0,fXR1,fYR1,fYR0;

			int i;

 

			switch(Format.wBitsPerSample)

			{

				case 8:

				switch(Format.nChannels)

				{

					case 1:

						fXL0=(float)*(byte *)lpData;

						fXL1=fXL0;

						fYL1=fXL0;

						for(i=0;i<dwDataLength;i+=sizeof(byte))

						{

							fXL0=(float)*(byte *)(lpData+i);

							fYL0=fParam0*fXL0+fParam1*fXL1-

								fParam2*fYL1;

							*(byte *)(lpData+i)=(byte)fYL0;

							fXL1=fXL0;

							fYL1=fYL0;

						}

						break;

					case 2:

						fXL0=(float)*(byte *)lpData;

						fXL1=fXL0;

						fYL1=fXL0;

						fXR0=(float)*(byte *)(lpData+sizeof(byte));

						fXR1=fXR0;

						fYR1=fXR0;

						for(i=0;i<dwDataLength;i+=2*sizeof(byte))

						{

							fXL0=(float)*(byte *)(lpData+i);

							fYL0=fParam0*fXL0+fParam1*fXL1-

								fParam2*fYL1;

							*(byte *)(lpData+i)=(byte)fYL0;

							fXL1=fXL0;

							fYL1=fYL0;

							fXR0=(float)*(byte *)(lpData+i+sizeof(byte));

							fYR0=fParam0*fXR0+fParam1*fXR1-

								fParam2*fYR1;

							*(byte *)(lpData+i+sizeof(byte))=(byte)fYR0;

							fXR1=fXR0;

							fYR1=fYR0;

						}

						break;

				}

					break;

				case 16:

				switch(Format.nChannels)

				{

					case 1:

						fXL0=(float)*(short *)lpData;

						fXL1=fXL0;

						fYL1=fXL0;

						for(i=0;i<dwDataLength;i+=2)

						{

							fXL0=(float)*(short *)(lpData+i);

							fYL0=fParam0*fXL0+fParam1*fXL1-

								fParam2*fYL1;

							*(short *)(lpData+i)=(short)fYL0;

							fXL1=fXL0;

							fYL1=fYL0;

						}

						break;

					case 2:

						fXL0=(float)*(short *)lpData;

						fXL1=fXL0;

						fYL1=fXL0;

						fXR0=(float)*(short *)(lpData+sizeof(short));

						fXR1=fXR0;

						fYR1=fXR0;

						for(i=0;i<dwDataLength;i+=2*sizeof(short))

						{

							fXL0=(float)*(short *)(lpData+i);

							fYL0=fParam0*fXL0+fParam1*fXL1-

								fParam2*fYL1;

							*(short *)(lpData+i)=(short)fYL0;

							fXL1=fXL0;

							fYL1=fYL0;

							fXR0=(float)*(short *)(lpData+

								i+sizeof(short));

							fYR0=fParam0*fXR0+fParam1*fXR1-

								fParam2*fYR1;

							*(short *)(lpData+i+sizeof(short))=

								(short)fYR0;

							fXR1=fXR0;

							fYR1=fYR0;

						}

						break;

				}

					break;

			}

		}
		#region

		#endregion

	}
}
