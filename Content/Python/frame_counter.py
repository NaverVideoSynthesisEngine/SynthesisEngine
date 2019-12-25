import os
import unreal


anim_list = os.listdir("D:\\ChangHun\\sourcetree\\SynthesisEngine\\MixamoDownload\\DownloadedAnimations\\Female")


def count(start, end):
	count = 0
	for i in anim_list[start:end]:
		anim_name = i[:-4]
		obj = unreal.load_asset("/Game/Animations/NewFBX/Female/{}".format(anim_name))
		count += unreal.AnimationLibrary.get_sequence_length(obj)
	print(count * 30)
	return count
