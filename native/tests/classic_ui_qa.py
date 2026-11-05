#!/usr/bin/env python3
"""Desktop native SDL/GLES input and lifecycle checks; no browser or TV claims."""
import argparse, json, os, shutil, subprocess, tempfile
from pathlib import Path

def main():
    p=argparse.ArgumentParser();p.add_argument('--binary',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    binary=a.binary.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    assets=Path(__file__).resolve().parents[1]/'assets'
    checks=[]
    def run(name,actions='',frames=20,width=1280,height=720,assetdir=assets,expected='playing',scores=None,save_override=None):
        directory=out/name;directory.mkdir(exist_ok=True)
        script=directory/'input.txt';script.write_text(actions)
        if scores is not None:(directory/'scores-v104.txt').write_text(scores)
        cmd=['xvfb-run','-a',str(binary),'--assets',str(assetdir),'--save',str(save_override or directory/'record.txt'),'--fixed-frame','--frames',str(frames),'--script',str(script),'--width',str(width),'--height',str(height),'--capture',str(directory/'frame.png'),'--report',str(directory/'report.json')]
        r=subprocess.run(cmd,capture_output=True,text=True,timeout=40,env={**os.environ,'LIBGL_ALWAYS_SOFTWARE':'1'})
        (directory/'log.txt').write_text(r.stdout+r.stderr)
        if r.returncode:raise AssertionError(name+': '+r.stdout+r.stderr)
        report=json.loads((directory/'report.json').read_text());assert report['screen']==expected,(name,report)
        checks.append(name);return report
    st=run('original_start');assert st['heading']==3 and st['distance']==0 and not st['fast'] and st['assets_ready'] and not st['original_hud_visible']
    st=run('downhill','1 down\n',frames=40);assert st['distance']>0 and st['original_hud_visible']
    st=run('hud_hidden_until_downhill','1 char:x\n');assert not st['original_hud_visible']
    st=run('hud_hidden_after_restart','1 down\n20 restart\n',frames=21);assert not st['original_hud_visible']
    st=run('fast_persistent','1 fast\n',frames=200);assert st['fast']
    st=run('fast_repeat_original','1 fast\n2 repeat-fast\n');assert not st['fast']
    st=run('uppercase_f_original','1 char:F\n');assert not st['fast']
    st=run('fast_off','1 fast\n2 fast\n');assert not st['fast']
    st=run('remote_ok_jump','1 ok\n',frames=8);assert st['in_air']>0
    st=run('insert_jump','1 jump\n',frames=8);assert st['in_air']>0
    st=run('keyboard_enter_original','1 scan:40\n',frames=8);assert st['in_air']==0 and st['heading']==3
    st=run('character_nudge','1 char:x\n',frames=6);assert st['player_x']==2
    st=run('character_nudge_back','1 char:x\n2 char:X\n',frames=6);assert st['player_x']==0
    baseline=run('paused_no_step','1 down\n10 back\n',expected='paused')
    st=run('paused_original_step','1 down\n10 back\n12 char:t\n',expected='paused');assert st['distance']>baseline['distance']
    st=run('pause_focus','1 down\n10 blur\n12 focus\n',expected='paused');assert st['elapsed']<0.2
    st=run('pause_hidden','1 down\n10 hide\n12 show\n',expected='paused');assert st['elapsed']<0.2
    st=run('back_resume','1 down\n10 back\n15 back\n',frames=40);assert st['distance']>0
    st=run('restart_from_pause','1 down\n10 back\n15 restart\n');assert st['distance']==0 and st['heading']==3
    st=run('pointer_then_keys','1 move:640:480\n2 move:644:484\n4 left\n5 move:644:484\n',frames=6);assert st['heading']==1 and not st['pointer_active']
    st=run('pointer_reclaims','1 move:640:480\n2 move:644:484\n4 left\n5 move:650:490\n',frames=6);assert st['heading']==0 and st['pointer_active']
    st=run('scaled_pointer','1 move:960:720\n2 move:964:724\n',frames=6,width=1920,height=1080);assert st['heading']==0
    st=run('letterbox_pointer','1 move:640:632\n2 move:644:636\n',frames=6,width=1280,height=1024);assert st['heading']==0
    st=run('menu_fast','1 back\n2 down\n3 down\n4 down\n5 ok\n6 back\n');assert st['fast']
    st=run('green_recreation','1 scan:487\n',frames=30);assert st['edition']==1 and st['recreation_distance']>0 and st['distance']==0
    st=run('yellow_chase','1 scan:488\n',frames=30);assert st['edition']==2 and st['chase_distance']>0 and st['distance']==0
    st=run('red_original','1 scan:487\n10 scan:486\n');assert st['edition']==0 and st['recreation_distance']>0
    st=run('native_red_fallback','1 scan:487\n10 red-keycode\n');assert st['edition']==0
    st=run('blue_help','1 scan:489\n',expected='help');assert st['edition']==0
    st=run('blue_toggle','1 scan:489\n5 scan:489\n');assert st['edition']==0
    st=run('recreation_help','1 scan:487\n2 scan:489\n',expected='help');assert st['edition_name']=='RECREATION'
    st=run('chase_help','1 scan:488\n2 scan:489\n',expected='help');assert st['edition_name']=='YETI CHASE'
    st=run('recreation_pause','1 scan:487\n10 back\n',expected='paused');assert st['edition']==1
    baseline=run('recreation_no_boost','1 scan:487\n',frames=30)
    st=run('recreation_menu_boost','1 scan:487\n2 back\n3 down\n4 down\n5 down\n6 ok\n',frames=30)
    assert st['edition']==1 and st['recreation_distance']>baseline['recreation_distance']
    baseline=run('original_freeze_baseline','1 down\n',frames=30)
    st=run('original_preserved','1 down\n30 scan:487\n90 scan:486\n',frames=91)
    assert st['distance']==baseline['distance'] and abs(st['elapsed']-baseline['elapsed'])<=1/60+0.001,(baseline,st)
    baseline=run('recreation_freeze_baseline','1 scan:487\n',frames=30)
    st=run('recreation_preserved','1 scan:487\n30 scan:486\n90 scan:487\n',frames=91)
    assert abs(st['recreation_distance']-baseline['recreation_distance'])<0.6,(baseline,st)
    st=run('click_green','1 click:450:685\n');assert st['edition']==1
    st=run('centered_green_hitbox','1 click:348:685\n');assert st['edition']==1
    st=run('versions_menu','1 back\n2 down\n3 down\n4 down\n5 down\n6 ok\n',expected='versions')
    st=run('versions_choose','1 back\n2 down\n3 down\n4 down\n5 down\n6 ok\n7 down\n8 ok\n');assert st['edition']==1
    st=run('intro_lift_clearance',frames=1200);assert st['original_intro'] and st['lift_offset']<0
    st=run('lift_gameplay_position','1 down\n',frames=200);assert not st['original_intro'] and st['lift_offset']==0
    st=run('score_current_row','1 score:SS:1000\n2 back\n',expected='scores',scores='skifree-v104\n0 1000\n0 2000\n');assert st['score_rank']==2
    st=run('score_outside_top_ten','1 score:SS:9999\n',expected='scores',scores='skifree-v104\n'+''.join(f'0 {i*100}\n' for i in range(1,11)));assert st['score_rank']==0 and st['score_value']==9999
    st=run('score_freestyle','1 score:FS:250\n2 back\n',expected='scores',scores='skifree-v104\n2 300\n2 200\n');assert st['score_rank']==2
    st=run('score_store_saved','1 score:SS:1000\n2 back\n',expected='scores');assert st['record_writable']
    saved=(out/'score_store_saved'/'scores-initials-v1.txt').read_text();assert saved=='skifree-initials-v1\n0 1000 AAA\n'
    reloaded=out/'score_store_reloaded';reloaded.mkdir(exist_ok=True)
    shutil.copy2(out/'score_store_saved'/'scores-initials-v1.txt',reloaded/'scores-initials-v1.txt')
    st=run('score_store_reloaded','1 score:SS:2000\n2 back\n',expected='scores');assert st['score_rank']==2
    assert (reloaded/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 AAA\n0 2000 AAA\n'
    damaged='skifree-v104\n0 1000\n0'
    st=run('score_store_invalid','1 score:SS:2000\n2 back\n',expected='scores',scores=damaged);assert not st['record_writable']
    assert (out/'score_store_invalid'/'scores-v104.txt').read_text()==damaged
    assert not (out/'score_store_invalid'/'scores-initials-v1.txt').exists()
    st=run('initials_entry','1 score:SS:12345\n',expected='initials');assert st['score_rank']==1
    st=run('initials_keyboard','1 score:SS:12345\n2 scan:21\n3 scan:28\n4 scan:17\n5 scan:40\n',expected='scores')
    assert (out/'initials_keyboard'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 12345 RYN\n'
    st=run('initials_remote','1 score:GS:23456\n2 up\n3 right\n4 down\n5 ok\n6 up\n7 ok\n8 ok\n',expected='scores')
    assert (out/'initials_remote'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n1 23456 BZB\n'
    st=run('initials_pointer','1 score:FS:300\n2 click:520:316\n3 click:640:450\n4 click:760:316\n5 click:640:580\n',expected='scores')
    assert (out/'initials_pointer'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n2 300 BZB\n'
    st=run('initials_backspace','1 score:SS:1000\n2 scan:21\n3 scan:28\n4 scan:42\n5 scan:4\n6 back\n',expected='scores')
    assert (out/'initials_backspace'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 RAA\n'
    st=run('initials_switch','1 score:SS:1000\n2 scan:21\n3 recreation\n');assert st['edition']==1
    assert (out/'initials_switch'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 RAA\n'
    st=run('initials_close_pending','1 score:SS:1000\n2 scan:21\n',expected='initials')
    assert (out/'initials_close_pending'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 RAA\n'
    st=run('initials_focus','1 score:SS:1000\n2 scan:21\n3 blur\n4 focus\n',expected='initials')
    assert (out/'initials_focus'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 RAA\n'
    st=run('initials_legacy_preserved','1 score:SS:1000\n2 back\n',expected='scores',scores='skifree-v104\n0 2000\n')
    assert (out/'initials_legacy_preserved'/'scores-v104.txt').read_text()=='skifree-v104\n0 2000\n'
    assert (out/'initials_legacy_preserved'/'scores-initials-v1.txt').read_text()=='skifree-initials-v1\n0 1000 AAA\n0 2000 ---\n'
    st=run('recreation_hidden_footer','1 recreation\n2 click:760:685\n');assert st['edition']==1
    st=run('recreation_light_hud','1 recreation\n2 fast\n',frames=60);assert st['edition']==1
    st=run('initials_tenth','1 score:SS:9999\n2 back\n',expected='scores',scores='skifree-v104\n'+''.join(f'0 {i*1000}\n' for i in range(1,10)));assert st['score_rank']==10
    browse='1 back\n'+''.join(f'{i+2} down\n' for i in range(5))+'7 ok\n'
    tables='skifree-v104\n0 1000\n1 2000\n2 300\n'
    st=run('browser_empty',browse,expected='scores');assert st['browsing_scores'] and st['score_course']==0 and st['score_counts']==[0,0,0]
    st=run('browser_next',browse+'8 right\n',expected='scores',scores=tables);assert st['score_course']==1 and st['score_counts']==[1,1,1]
    st=run('browser_previous_wrap',browse+'8 left\n',expected='scores',scores=tables);assert st['score_course']==2
    st=run('browser_pointer',browse+'8 click:640:580\n',expected='scores',scores=tables);assert st['score_course']==1
    st=run('browser_back',browse+'8 back\n',expected='paused');assert st['browsing_scores']
    st=run('browser_resume',browse+'8 back\n9 back\n');assert st['screen']=='playing'
    st=run('browser_recreation','0 recreation\n'+browse,expected='scores',scores=tables);assert st['edition']==1 and st['browsing_scores']
    st=run('pause_exit_after_browser_added','1 back\n'+''.join(f'{i+2} down\n' for i in range(6))+'8 ok\n',expected='exit-confirm')
    st=run('browser_saved_session','1 score:SS:12345\n2 char:A\n3 char:B\n4 char:C\n5 ok\n6 back\n7 score:FS:250\n8 char:X\n9 char:Y\n10 char:Z\n11 ok\n',expected='scores');assert st['score_counts']==[1,0,1] and not st['browsing_scores']
    saved=out/'browser_saved_session'/'scores-initials-v1.txt';contents=saved.read_text()
    assert contents=='skifree-initials-v1\n0 12345 ABC\n2 250 XYZ\n'
    st=run('browser_saved_reopened',browse,expected='scores',save_override=saved.parent/'record.txt');assert st['score_counts']==[1,0,1] and st['score_course']==0 and st['browsing_scores']
    st=run('browser_saved_freestyle',browse+'8 left\n',expected='scores',save_override=saved.parent/'record.txt');assert st['score_course']==2 and st['score_counts']==[1,0,1] and saved.read_text()==contents
    # Reach the real Slalom entrance through original character controls;
    # the normal downhill movement then crosses the start gate.
    route=''.join(f'{i+1} char:X\n' for i in range(224))
    route+=''.join(f'{225+i} char:y\n' for i in range(319))+'546 down\n'
    baseline=run('slalom_timer_running',route,frames=650);assert baseline['course']==1 and baseline['race_elapsed_ms']>0
    st=run('slalom_saved_record_hud',route,frames=650,scores='skifree-v104\n0 12345\n');assert st['original_hud_visible'] and st['course']==1 and st['original_hud_best']=='BEST: --- 00:00:12.34'
    st=run('slalom_timer_paused',route+'650 back\n',frames=720,expected='paused');assert st['race_elapsed_ms']==baseline['race_elapsed_ms']
    st=run('slalom_timer_browsing',route+'650 back\n'+''.join(f'{651+i} down\n' for i in range(5))+'656 ok\n',frames=720,expected='scores');assert st['browsing_scores'] and st['race_elapsed_ms']==baseline['race_elapsed_ms']
    st=run('slalom_timer_resumed',route+'650 back\n700 back\n',frames=760);assert st['race_elapsed_ms']>baseline['race_elapsed_ms']
    st=run('startup_unwritable','1 back\n',expected='paused',save_override='/proc/skifree-unwritable/record.txt');assert not st['record_writable']
    with tempfile.TemporaryDirectory(prefix='skifree-ui-') as temp:
        temporary=Path(temp);run('missing_assets',assetdir=temporary,expected='setup')
        shutil.copytree(assets,temporary,dirs_exist_ok=True);(temporary/'ski32_001.png').unlink()
        run('partial_assets',assetdir=temporary,expected='setup')
        (temporary/'ski32_001.png').write_bytes(b'not a png')
        run('corrupt_assets',assetdir=temporary,expected='setup')
    (out/'summary.json').write_text(json.dumps({'passed':len(checks),'checks':checks,'scope':'desktop native software GLES; not physical TV input acceptance'},indent=2)+'\n')
    print('PASS:',len(checks),'native UI checks')
if __name__=='__main__':main()
