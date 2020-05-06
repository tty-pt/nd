import { kmem } from './env.js';
import { umem } from './env.js';
import { flush } from './env.js';
import { js_emem } from './env.js';
import { js_shutdown } from './env.js';
import { evt_count } from './env.js';
import { js_run } from './env.js';

function asmFunc(global, env, buffer) {
 var HEAP8 = new global.Int8Array(buffer);
 var HEAP16 = new global.Int16Array(buffer);
 var HEAP32 = new global.Int32Array(buffer);
 var HEAPU8 = new global.Uint8Array(buffer);
 var HEAPU16 = new global.Uint16Array(buffer);
 var HEAPU32 = new global.Uint32Array(buffer);
 var HEAPF32 = new global.Float32Array(buffer);
 var HEAPF64 = new global.Float64Array(buffer);
 var Math_imul = global.Math.imul;
 var Math_fround = global.Math.fround;
 var Math_abs = global.Math.abs;
 var Math_clz32 = global.Math.clz32;
 var Math_min = global.Math.min;
 var Math_max = global.Math.max;
 var Math_floor = global.Math.floor;
 var Math_ceil = global.Math.ceil;
 var Math_sqrt = global.Math.sqrt;
 var abort = env.abort;
 var nan = global.NaN;
 var infinity = global.Infinity;
 var kmem = env.kmem;
 var umem = env.umem;
 var flush = env.flush;
 var js_emem = env.js_emem;
 var js_shutdown = env.js_shutdown;
 var evt_count = env.evt_count;
 var js_run = env.js_run;
 var global$0 = 208128;
 var global$1 = 208128;
 var global$2 = 142588;
 var i64toi32_i32$HIGH_BITS = 0;
 function do_early_param($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $2 = 83004;
  while (1) {
   label$3 : {
    label$4 : {
     if (HEAP32[$2 + 8 >> 2]) {
      if (parameq($0, HEAP32[$2 >> 2])) {
       break label$4
      }
     }
     if (strcmp($0, 20204)) {
      break label$3
     }
     if (strcmp(HEAP32[$2 >> 2], 20212)) {
      break label$3
     }
    }
    if (!FUNCTION_TABLE[HEAP32[$2 + 4 >> 2]]($1)) {
     break label$3
    }
    HEAP32[$3 >> 2] = $0;
    printk(20221, $3);
   }
   $2 = $2 + 12 | 0;
   if ($2 >>> 0 < 83008) {
    continue
   }
   break;
  };
  global$0 = $3 + 16 | 0;
  return 0;
 }
 
 function parse_early_param() {
  if (!HEAPU8[140044]) {
   strlcpy();
   parse_args(20013, 140048, 2);
   HEAP8[140044] = 1;
  }
 }
 
 function start_kernel() {
  var $0 = 0, $1 = 0;
  $1 = global$0 - 48 | 0;
  global$0 = $1;
  HEAP32[HEAP32[257] + 24 >> 2] = 1470918301;
  HEAP8[65540] = 1;
  HEAP8[65556] = 0;
  boot_cpu_init();
  HEAP32[$1 + 32 >> 2] = 19936;
  printk(20027, $1 + 32 | 0);
  HEAP32[$1 + 44 >> 2] = 140560;
  parse_early_param();
  setup_processor();
  setup_arch_memory();
  $0 = HEAP32[$1 + 44 >> 2];
  add_device_randomness($0, strlen($0));
  memset(16136, 0, 4);
  printk(20032, 0);
  HEAP8[8492] = 1;
  build_all_zonelists();
  __cpuhp_setup_state_cpuslocked();
  HEAP32[$1 + 16 >> 2] = 140560;
  printk(20109, $1 + 16 | 0);
  parse_early_param();
  $0 = parse_args(20136, 0, 3);
  label$1 : {
   if (!$0) {
    break label$1
   }
   if ($0 >>> 0 > 4294963200) {
    break label$1
   }
   parse_args(20151, $0, 4);
  }
  HEAP8[65552] = 1;
  setup_log_buf();
  mem_init();
  HEAP32[20650] = 3;
  HEAP32[20649] = 15616;
  vmalloc_init();
  sched_init();
  if (HEAPU8[65556]) {
   HEAP8[65556] = 0
  }
  HEAP32[16413] = 20;
  HEAP32[2145] = HEAP32[20745];
  HEAP32[2147] = 0;
  HEAP32[16405] = 12;
  hrtimers_prepare_cpu();
  HEAP32[16412] = 13;
  HEAP32[1124] = 4492;
  HEAP32[1122] = 4484;
  HEAP32[16410] = 7;
  HEAP32[16404] = 8;
  timekeeping_init();
  HEAP32[2989] = 22;
  HEAP32[2987] = 0;
  HEAP8[79296] = 1;
  __printk_safe_flush(11948);
  HEAP8[65556] = 1;
  HEAP8[65540] = 0;
  HEAP32[20650] = 4;
  $0 = HEAP32[16386];
  if (!$0) {
   $0 = HEAP32[35268];
   if ($0) {
    FUNCTION_TABLE[$0]()
   }
   HEAP32[19830] = HEAP32[19830] + 1;
   global$0 = $1 + 48 | 0;
   return;
  }
  HEAP32[$1 >> 2] = $0;
  HEAP32[$1 + 4 >> 2] = HEAP32[16387];
  panic(20169, $1);
  abort();
 }
 
 function unknown_bootoption($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0;
  repair_env_string($0, $1);
  label$1 : {
   label$2 : {
    if (obsolete_checksetup($0)) {
     break label$2
    }
    if (strchr($0)) {
     if (!$1) {
      break label$2
     }
     if (strchr($0) >>> 0 < $1 >>> 0) {
      break label$2
     }
    }
    if (!HEAP32[16386]) {
     break label$1
    }
   }
   return 0;
  }
  label$4 : {
   label$5 : {
    label$6 : {
     label$7 : {
      if ($1) {
       $3 = HEAP32[880];
       if (!$3) {
        break label$7
       }
       $4 = $1 - $0 | 0;
       $2 = 3520;
       $1 = 8;
       while (1) {
        if (!$1) {
         HEAP32[16387] = $0;
         HEAP32[16386] = 20252;
        }
        if (!strncmp($0, $3, $4)) {
         break label$6
        }
        $1 = $1 + -1 | 0;
        $2 = $2 + 4 | 0;
        $3 = HEAP32[$2 >> 2];
        if ($3) {
         continue
        }
        break;
       };
       break label$6;
      }
      if (!HEAP32[892]) {
       break label$5
      }
      $1 = 8;
      $2 = 3568;
      while (1) {
       if (!$1) {
        HEAP32[16387] = $0;
        HEAP32[16386] = 20199;
       }
       $1 = $1 + -1 | 0;
       $2 = $2 + 4 | 0;
       if (HEAP32[$2 >> 2]) {
        continue
       }
       break;
      };
      break label$4;
     }
     $2 = 3520;
    }
    HEAP32[$2 >> 2] = $0;
    return 0;
   }
   $2 = 3568;
  }
  HEAP32[$2 >> 2] = $0;
  return 0;
 }
 
 function set_init_arg($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  if (HEAP32[16386]) {
   return 0
  }
  repair_env_string($0, $1);
  label$2 : {
   label$3 : {
    if (HEAP32[892]) {
     $1 = 0;
     while (1) {
      $2 = $1 + 4 | 0;
      if (($2 | 0) == 36) {
       break label$3
      }
      $3 = $1 + 3572 | 0;
      $1 = $2;
      if (HEAP32[$3 >> 2]) {
       continue
      }
      break;
     };
     $1 = $1 + 3568 | 0;
     break label$2;
    }
    $1 = 3568;
    break label$2;
   }
   HEAP32[16386] = 20199;
   $1 = 65548;
  }
  HEAP32[$1 >> 2] = $0;
  return 0;
 }
 
 function repair_env_string($0, $1) {
  var $2 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  label$1 : {
   label$2 : {
    if (!$1) {
     break label$2
    }
    if (($1 | 0) != ((strlen($0) + $0 | 0) + 1 | 0)) {
     if (((strlen($0) + $0 | 0) + 2 | 0) != ($1 | 0)) {
      break label$1
     }
     HEAP8[$1 + -2 | 0] = 61;
     memmove($1 + -1 | 0, $1, strlen($1) + 1 | 0);
     break label$2;
    }
    HEAP8[$1 + -1 | 0] = 61;
   }
   global$0 = $2 + 16 | 0;
   return;
  }
  HEAP32[$2 + 8 >> 2] = 20294;
  HEAP32[$2 + 4 >> 2] = 261;
  HEAP32[$2 >> 2] = 20287;
  printk(20256, $2);
  abort();
 }
 
 function obsolete_checksetup($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $1 = 83004;
  label$1 : {
   label$2 : {
    while (1) {
     label$3 : {
      $2 = strlen(HEAP32[$1 >> 2]);
      label$5 : {
       if (!parameqn($0, HEAP32[$1 >> 2], $2)) {
        break label$5
       }
       if (HEAP32[$1 + 8 >> 2]) {
        $2 = HEAPU8[$0 + $2 | 0];
        if ($2 ? ($2 | 0) != 61 : 0) {
         break label$5
        }
        $4 = 1;
        break label$5;
       }
       $5 = HEAP32[$1 + 4 >> 2];
       if (!$5) {
        break label$3
       }
       if (FUNCTION_TABLE[$5]($0 + $2 | 0)) {
        break label$2
       }
      }
      $1 = $1 + 12 | 0;
      if ($1 >>> 0 < 83008) {
       continue
      }
      break label$1;
     }
     break;
    };
    HEAP32[$3 >> 2] = HEAP32[$1 >> 2];
    printk(20312, $3);
   }
   $4 = 1;
  }
  global$0 = $3 + 16 | 0;
  return $4;
 }
 
 function irq_work_queue($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $2 = HEAP32[$0 >> 2];
  $3 = $2 & 1;
  $1 = $2 & -2;
  label$1 : {
   while (1) {
    if (($1 | 0) == ($2 | 0)) {
     break label$1
    }
    $1 = $2;
    if (!$3) {
     continue
    }
    break;
   };
   return;
  }
  HEAP32[$0 >> 2] = $2 | 3;
  $1 = $0 + 4 | 0;
  label$3 : {
   if (!(HEAPU8[$0 | 0] & 4)) {
    HEAP32[$1 >> 2] = HEAP32[903];
    HEAP32[903] = $1;
    break label$3;
   }
   HEAP32[$1 >> 2] = HEAP32[902];
   HEAP32[902] = $1;
  }
 }
 
 function get_symbol_pos() {
  var $0 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 + 8 >> 2] = 20437;
  HEAP32[$0 + 4 >> 2] = 206;
  HEAP32[$0 >> 2] = 20426;
  printk(20395, $0);
  abort();
 }
 
 function kallsyms_lookup($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  HEAP8[$2 | 0] = 0;
  HEAP8[$2 + 127 | 0] = 0;
  label$1 : {
   if (!($0 >>> 0 >= 82968 ? $0 >>> 0 <= 19428 : 0)) {
    if ($0 >>> 0 > 82976) {
     break label$1
    }
    if ($0 >>> 0 < 82972) {
     break label$1
    }
   }
   $0 = get_symbol_pos();
   $3 = HEAP32[($0 >>> 6 & 67108860) >> 2];
   $0 = $0 & 255;
   if ($0) {
    while (1) {
     $3 = (HEAPU8[$3 | 0] + $3 | 0) + 1 | 0;
     $0 = $0 + -1 | 0;
     if ($0) {
      continue
     }
     break;
    }
   }
   $5 = $2;
   $6 = HEAPU8[$3 | 0];
   label$6 : {
    if ($6) {
     $7 = 128;
     while (1) {
      label$8 : {
       $0 = HEAPU16[HEAPU8[$3 + 1 | 0] << 1 >> 1];
       $4 = HEAPU8[$0 | 0];
       if ($4) {
        while (1) {
         label$12 : {
          if ($8) {
           if ($7 >>> 0 < 2) {
            break label$8
           }
           HEAP8[$5 | 0] = $4;
           $7 = $7 + -1 | 0;
           $5 = $5 + 1 | 0;
           break label$12;
          }
          $8 = 1;
         }
         $4 = HEAPU8[$0 | 0];
         $0 = $0 + 1 | 0;
         if ($4) {
          continue
         }
         break;
        }
       }
       $3 = $3 + 1 | 0;
       $6 = $6 + -1 | 0;
       if ($6) {
        continue
       }
      }
      break;
     };
     if (!$7) {
      break label$6
     }
    }
    HEAP8[$5 | 0] = 0;
   }
   if ($1) {
    HEAP32[$1 >> 2] = 0
   }
   $4 = $2;
  }
  return $4;
 }
 
 function __sprint_symbol($0, $1, $2, $3) {
  var $4 = 0, $5 = 0;
  $4 = global$0 + -64 | 0;
  global$0 = $4;
  $5 = kallsyms_lookup($1 + $2 | 0, $4 + 60 | 0, $0);
  label$1 : {
   if ($5) {
    if (($0 | 0) != ($5 | 0)) {
     strcpy($0, $5)
    }
    $1 = strlen($0);
    $2 = HEAP32[$4 + 56 >> 2] - $2 | 0;
    HEAP32[$4 + 56 >> 2] = $2;
    if ($3) {
     HEAP32[$4 + 36 >> 2] = HEAP32[$4 + 52 >> 2];
     HEAP32[$4 + 32 >> 2] = $2;
     $1 = sprintf($0 + $1 | 0, 20458, $4 + 32 | 0) + $1 | 0;
    }
    $2 = HEAP32[$4 + 60 >> 2];
    if (!$2) {
     break label$1
    }
    HEAP32[$4 + 16 >> 2] = $2;
    sprintf($0 + $1 | 0, 20469, $4 + 16 | 0);
    break label$1;
   }
   HEAP32[$4 >> 2] = $1;
   sprintf($0, 20452, $4);
  }
  global$0 = $4 - -64 | 0;
 }
 
 function put_cred_rcu($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  $0 = $0 + -92 | 0;
  if (!HEAP32[$0 >> 2]) {
   $2 = HEAP32[$0 + 88 >> 2];
   if ($2) {
    HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + -1
   }
   free_uid(HEAP32[$0 + 80 >> 2]);
   kmem_cache_free(HEAP32[16390], $0);
   global$0 = $1 + 16 | 0;
   return;
  }
  HEAP32[$1 + 4 >> 2] = HEAP32[$0 >> 2];
  HEAP32[$1 >> 2] = $0;
  panic(20524, $1);
  abort();
 }
 
 function exit_creds($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 96 | 0;
  global$0 = $1;
  $2 = HEAP32[$0 + 692 >> 2];
  HEAP32[$0 + 692 >> 2] = 0;
  $3 = HEAP32[$2 >> 2] + -1 | 0;
  HEAP32[$2 >> 2] = $3;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        if (!$3) {
         if (HEAP32[$2 >> 2]) {
          break label$6
         }
         $3 = HEAP32[2];
         if (($2 | 0) == HEAP32[$3 + 696 >> 2]) {
          break label$5
         }
         if (($2 | 0) == HEAP32[$3 + 692 >> 2]) {
          break label$4
         }
         call_rcu($2 + 92 | 0, 5);
        }
        $2 = HEAP32[$0 + 696 >> 2];
        HEAP32[$0 + 696 >> 2] = 0;
        $0 = HEAP32[$2 >> 2] + -1 | 0;
        HEAP32[$2 >> 2] = $0;
        if (!$0) {
         if (HEAP32[$2 >> 2]) {
          break label$3
         }
         $0 = HEAP32[2];
         if (($2 | 0) == HEAP32[$0 + 696 >> 2]) {
          break label$2
         }
         if (($2 | 0) == HEAP32[$0 + 692 >> 2]) {
          break label$1
         }
         call_rcu($2 + 92 | 0, 5);
        }
        global$0 = $1 + 96 | 0;
        return;
       }
       HEAP32[$1 + 88 >> 2] = 20513;
       HEAP32[$1 + 84 >> 2] = 141;
       HEAP32[$1 + 80 >> 2] = 20506;
       printk(20475, $1 + 80 | 0);
       abort();
      }
      HEAP32[$1 + 8 >> 2] = 20513;
      HEAP32[$1 + 4 >> 2] = 147;
      HEAP32[$1 >> 2] = 20506;
      printk(20475, $1);
      abort();
     }
     HEAP32[$1 + 24 >> 2] = 20513;
     HEAP32[$1 + 20 >> 2] = 148;
     HEAP32[$1 + 16 >> 2] = 20506;
     printk(20475, $1 + 16 | 0);
     abort();
    }
    HEAP32[$1 + 72 >> 2] = 20513;
    HEAP32[$1 + 68 >> 2] = 141;
    HEAP32[$1 + 64 >> 2] = 20506;
    printk(20475, $1 - -64 | 0);
    abort();
   }
   HEAP32[$1 + 40 >> 2] = 20513;
   HEAP32[$1 + 36 >> 2] = 147;
   HEAP32[$1 + 32 >> 2] = 20506;
   printk(20475, $1 + 32 | 0);
   abort();
  }
  HEAP32[$1 + 56 >> 2] = 20513;
  HEAP32[$1 + 52 >> 2] = 148;
  HEAP32[$1 + 48 >> 2] = 20506;
  printk(20475, $1 + 48 | 0);
  abort();
 }
 
 function atomic_notifier_call_chain($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $0 = HEAP32[$0 >> 2];
  if ($0) {
   while (1) {
    label$2 : {
     $3 = HEAP32[$0 + 4 >> 2];
     $4 = FUNCTION_TABLE[HEAP32[$0 >> 2]]($0, $1, $2) | 0;
     if (!$3) {
      break label$2
     }
     $0 = $3;
     if (!($4 & 32768)) {
      continue
     }
    }
    break;
   }
  }
 }
 
 function blocking_notifier_call_chain($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  label$1 : {
   if (HEAP32[4262]) {
    __down_read(17036);
    $1 = HEAP32[4262];
    if (!$1) {
     break label$1
    }
    while (1) {
     label$3 : {
      $2 = HEAP32[$1 + 4 >> 2];
      $3 = FUNCTION_TABLE[HEAP32[$1 >> 2]]($1, 0, $0) | 0;
      if (!$2) {
       break label$3
      }
      $1 = $2;
      if (!($3 & 32768)) {
       continue
      }
     }
     break;
    };
    __up_read(17036);
    return;
   }
   return;
  }
  __up_read(17036);
 }
 
 function sys_ni_syscall() {
  return -38;
 }
 
 function kthread_probe_data($0) {
  var $1 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  $0 = HEAP32[$0 + 604 >> 2];
  HEAP32[$1 + 12 >> 2] = 0;
  __probe_kernel_read($1 + 12 | 0, $0 + 8 | 0, 4);
  global$0 = $1 + 16 | 0;
  return HEAP32[$1 + 12 >> 2];
 }
 
 function parameqn($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  label$1 : {
   if ($2) {
    while (1) {
     $3 = HEAPU8[$0 + $4 | 0];
     $5 = ($3 | 0) == 45 ? 95 : $3;
     $3 = HEAPU8[$1 + $4 | 0];
     if (($5 | 0) != ((($3 | 0) == 45 ? 95 : $3) | 0)) {
      break label$1
     }
     $4 = $4 + 1 | 0;
     if ($4 >>> 0 < $2 >>> 0) {
      continue
     }
     break;
    };
    return 1;
   }
   return 1;
  }
  return 0;
 }
 
 function parameq($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $4 = 1;
  $5 = strlen($0) + 1 | 0;
  label$1 : {
   if (!$5) {
    break label$1
   }
   $4 = 0;
   while (1) {
    $2 = HEAPU8[$0 + $3 | 0];
    $6 = ($2 | 0) == 45 ? 95 : $2;
    $2 = HEAPU8[$1 + $3 | 0];
    if (($6 | 0) != ((($2 | 0) == 45 ? 95 : $2) | 0)) {
     break label$1
    }
    $3 = $3 + 1 | 0;
    if ($3 >>> 0 < $5 >>> 0) {
     continue
    }
    break;
   };
   $4 = 1;
  }
  return $4;
 }
 
 function parse_args($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $3 = global$0 - 128 | 0;
  global$0 = $3;
  label$1 : {
   label$2 : {
    label$3 : {
     $4 = skip_spaces($1);
     if (HEAPU8[$4 | 0]) {
      if (HEAPU8[141194] & 4) {
       break label$3
      }
      break label$2;
     }
     break label$1;
    }
    HEAP32[$3 + 112 >> 2] = $0;
    HEAP32[$3 + 116 >> 2] = $4;
    __dynamic_pr_debug(141176, 20594, $3 + 112 | 0);
    if (!HEAPU8[$4 | 0]) {
     break label$1
    }
   }
   while (1) {
    $4 = next_arg($4, $3 + 124 | 0, $3 + 120 | 0);
    label$6 : {
     label$7 : {
      label$8 : {
       label$9 : {
        $1 = HEAP32[$3 + 120 >> 2];
        $6 = HEAP32[$3 + 124 >> 2];
        label$12 : {
         if ($1) {
          break label$12
         }
         if (!strcmp(HEAP32[$3 + 124 >> 2], 20624)) {
          break label$9
         }
         $1 = HEAP32[$3 + 120 >> 2];
         $6 = HEAP32[$3 + 124 >> 2];
        }
        if (!$2) {
         if (!(HEAPU8[141266] & 4)) {
          break label$7
         }
         HEAP32[$3 + 16 >> 2] = $6;
         __dynamic_pr_debug(141248, 20788, $3 + 16 | 0);
         break label$7;
        }
        label$10 : {
         if (HEAPU8[141242] & 4) {
          HEAP32[$3 + 72 >> 2] = $1;
          HEAP32[$3 + 68 >> 2] = $6;
          HEAP32[$3 + 64 >> 2] = $0;
          __dynamic_pr_debug(141224, 20769, $3 - -64 | 0);
          $1 = FUNCTION_TABLE[$2]($6, $1, $0, 0) | 0;
          if (!$1) {
           break label$10
          }
          break label$8;
         }
         $1 = FUNCTION_TABLE[$2]($6, $1, $0, 0) | 0;
         if (!$1) {
          break label$10
         }
         break label$8;
        }
        if (HEAPU8[$4 | 0]) {
         continue
        }
        break label$1;
       }
       $5 = $5 ? $5 : $4;
       break label$1;
      }
      if (($1 | 0) == -28) {
       HEAP32[$3 + 56 >> 2] = HEAP32[$3 + 124 >> 2];
       HEAP32[$3 + 48 >> 2] = $0;
       $1 = HEAP32[$3 + 120 >> 2];
       HEAP32[$3 + 52 >> 2] = $1 ? $1 : 20698;
       printk(20657, $3 + 48 | 0);
       $5 = -28;
       if (HEAPU8[$4 | 0]) {
        continue
       }
       break label$1;
      }
      if (($1 | 0) != -2) {
       break label$6
      }
     }
     HEAP32[$3 + 4 >> 2] = HEAP32[$3 + 124 >> 2];
     HEAP32[$3 >> 2] = $0;
     printk(20627, $3);
     $5 = -2;
     if (HEAPU8[$4 | 0]) {
      continue
     }
     break label$1;
    }
    HEAP32[$3 + 40 >> 2] = HEAP32[$3 + 124 >> 2];
    HEAP32[$3 + 32 >> 2] = $0;
    $5 = HEAP32[$3 + 120 >> 2];
    HEAP32[$3 + 36 >> 2] = $5 ? $5 : 20698;
    printk(20699, $3 + 32 | 0);
    $5 = $1;
    if (HEAPU8[$4 | 0]) {
     continue
    }
    break;
   };
  }
  global$0 = $3 + 128 | 0;
  return $5;
 }
 
 function __task_pid_nr_ns($0, $1, $2) {
  var $3 = 0;
  label$1 : {
   if ($2) {
    break label$1
   }
   $2 = 0;
   $3 = HEAP32[HEAP32[2] + 548 >> 2];
   if (!$3) {
    break label$1
   }
   $2 = HEAP32[((HEAP32[$3 + 4 >> 2] << 3) + $3 | 0) + 36 >> 2];
  }
  label$2 : {
   label$3 : {
    if (!HEAP32[$0 + 548 >> 2]) {
     break label$3
    }
    label$4 : {
     if ($1) {
      $0 = HEAP32[(HEAP32[$0 + 732 >> 2] + ($1 << 2) | 0) + 72 >> 2];
      if ($0) {
       break label$4
      }
      break label$3;
     }
     $0 = HEAP32[$0 + 548 >> 2];
     if (!$0) {
      break label$3
     }
    }
    $1 = HEAP32[$2 + 40 >> 2];
    if ($1 >>> 0 > HEAPU32[$0 + 4 >> 2]) {
     break label$3
    }
    $0 = ($1 << 3) + $0 | 0;
    if (HEAP32[$0 + 36 >> 2] == ($2 | 0)) {
     break label$2
    }
   }
   return 0;
  }
  return HEAP32[$0 + 32 >> 2];
 }
 
 function wq_worker_sleeping($0) {
  var $1 = 0;
  label$1 : {
   $0 = HEAP32[HEAP32[$0 + 604 >> 2] + 8 >> 2];
   if (HEAPU16[$0 + 48 >> 1] & 456) {
    break label$1
   }
   $0 = HEAP32[$0 + 32 >> 2];
   if (HEAP32[$0 >> 2]) {
    break label$1
   }
   $1 = HEAP32[$0 + 380 >> 2] + -1 | 0;
   HEAP32[$0 + 380 >> 2] = $1;
   if ($1) {
    break label$1
   }
   if (HEAP32[$0 + 20 >> 2] == ($0 + 20 | 0)) {
    break label$1
   }
   $1 = HEAP32[$0 + 36 >> 2];
   if (!$1) {
    break label$1
   }
   if (($1 | 0) == ($0 + 36 | 0)) {
    break label$1
   }
   return HEAP32[$1 + 28 >> 2];
  }
  return 0;
 }
 
 function queue_work_on($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $5 = HEAP32[16394];
  HEAP32[16394] = 0;
  $4 = HEAP32[$2 >> 2];
  HEAP32[$3 + 12 >> 2] = $4;
  label$1 : {
   if (HEAP32[$3 + 12 >> 2] & 1) {
    break label$1
   }
   HEAP32[$2 >> 2] = $4 | 1;
   if ($4 & 1) {
    break label$1
   }
   __queue_work($0, $1, $2);
  }
  HEAP32[16394] = $5;
  global$0 = $3 + 16 | 0;
 }
 
 function __queue_work($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  label$1 : {
   if (HEAP8[$1 + 130 | 0] & 1) {
    if (HEAP32[1] & 2031872) {
     break label$1
    }
    $3 = HEAP32[2];
    if (!(HEAPU8[$3 + 12 | 0] & 32)) {
     break label$1
    }
    $3 = HEAP32[HEAP32[$3 + 604 >> 2] + 8 >> 2];
    if (!$3) {
     break label$1
    }
    if (HEAP32[HEAP32[$3 + 16 >> 2] + 4 >> 2] != ($1 | 0)) {
     break label$1
    }
   }
   $4 = $1 + 128 | 0;
   $5 = $1 + 132 | 0;
   $7 = Math_imul($2, 1640531527) >>> 26 << 2;
   $8 = $2 + 12 | 0;
   $9 = $1 + 136 | 0;
   $6 = ($0 | 0) != 1;
   $0 = 0;
   label$3 : {
    if (!$6) {
     break label$3
    }
    $0 = 1;
   }
   while (1) {
    label$5 : {
     label$7 : {
      label$8 : {
       label$9 : {
        label$10 : {
         label$11 : {
          if (!$0) {
           if (HEAPU8[65564] == 1) {
            break label$11
           }
           $0 = 1;
           continue;
          }
          $3 = HEAPU8[$4 | 0] & 2 ? HEAP32[$9 >> 2] : HEAP32[$5 >> 2];
          $0 = HEAP32[$2 >> 2];
          label$16 : {
           label$17 : {
            label$21 : {
             if (!($0 & 4)) {
              $0 = $0 >>> 5 | 0;
              if (($0 | 0) == 134217727) {
               break label$17
              }
              $0 = idr_find($0);
              if ($0) {
               break label$21
              }
              break label$17;
             }
             $0 = HEAP32[($0 & -256) >> 2];
             if (!$0) {
              break label$17
             }
            }
            if (HEAP32[$3 >> 2] == ($0 | 0)) {
             break label$17
            }
            $0 = HEAP32[($0 + $7 | 0) + 84 >> 2];
            if (!$0) {
             break label$17
            }
            label$19 : {
             while (1) {
              if (HEAP32[$0 + 12 >> 2] == HEAP32[$8 >> 2] ? ($2 | 0) == HEAP32[$0 + 8 >> 2] : 0) {
               break label$19
              }
              $0 = HEAP32[$0 >> 2];
              if ($0) {
               continue
              }
              break;
             };
             break label$17;
            }
            $0 = HEAP32[$0 + 16 >> 2];
            if (HEAP32[$0 + 4 >> 2] == ($1 | 0)) {
             break label$16
            }
           }
           $0 = $3;
          }
          if (HEAP32[$0 + 16 >> 2]) {
           break label$10
          }
          if (!(HEAPU8[$4 | 0] & 2)) {
           break label$10
          }
          if ($6) {
           break label$9
          }
          $0 = 0;
          continue;
         }
         if (HEAPU8[65580]) {
          break label$8
         }
         printk(20917, 0);
         HEAP8[65580] = 1;
         break label$7;
        }
        label$25 : {
         $5 = $2 + 4 | 0;
         if (($5 | 0) == HEAP32[$2 + 4 >> 2]) {
          $1 = ((HEAP32[$0 + 8 >> 2] << 2) + $0 | 0) + 20 | 0;
          HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
          $4 = HEAP32[$0 + 8 >> 2] << 4;
          $1 = HEAP32[$0 + 80 >> 2];
          if (($1 | 0) >= HEAP32[$0 + 84 >> 2]) {
           break label$25
          }
          HEAP32[$0 + 80 >> 2] = $1 + 1;
          $1 = HEAP32[$0 >> 2];
          $3 = $1 + 20 | 0;
          if (($3 | 0) != HEAP32[$1 + 20 >> 2]) {
           break label$5
          }
          HEAP32[$1 + 16 >> 2] = HEAP32[20745];
          break label$5;
         }
         return;
        }
        $3 = $0 + 88 | 0;
        $4 = $4 | 2;
        $1 = HEAP32[$0 >> 2];
        break label$5;
       }
       $0 = 1;
       continue;
      }
      $0 = 1;
      continue;
     }
     $0 = 1;
     continue;
    }
    break;
   };
   HEAP32[$2 >> 2] = $0 | $4 | 5;
   $4 = HEAP32[$3 + 4 >> 2];
   HEAP32[$3 + 4 >> 2] = $5;
   HEAP32[$4 >> 2] = $5;
   HEAP32[$2 + 4 >> 2] = $3;
   HEAP32[$2 + 8 >> 2] = $4;
   $0 = $0 + 16 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
   label$27 : {
    if (HEAP32[$1 + 380 >> 2]) {
     break label$27
    }
    $0 = HEAP32[$1 + 36 >> 2];
    if (!$0) {
     break label$27
    }
    if (($0 | 0) == ($1 + 36 | 0)) {
     break label$27
    }
    wake_up_process(HEAP32[$0 + 28 >> 2]);
   }
  }
 }
 
 function mod_delayed_work_on($0, $1) {
  var $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  while (1) {
   $3 = try_to_grab_pending($1, $2 + 12 | 0);
   if (($3 | 0) == -11) {
    continue
   }
   break;
  };
  if (($3 | 0) >= 0) {
   __queue_work(1, $0, $1);
   HEAP32[16394] = HEAP32[$2 + 12 >> 2];
  }
  global$0 = $2 + 16 | 0;
 }
 
 function try_to_grab_pending($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $2 = HEAP32[16394];
  HEAP32[16394] = 0;
  HEAP32[$1 >> 2] = $2;
  $3 = 1;
  label$1 : {
   if (del_timer($0 + 16 | 0)) {
    break label$1
   }
   $2 = HEAP32[$0 >> 2];
   HEAP32[$7 + 12 >> 2] = $2;
   if (!(HEAP32[$7 + 12 >> 2] & 1)) {
    HEAP32[$0 >> 2] = $2 | 1;
    $3 = 0;
    if (!($2 & 1)) {
     break label$1
    }
   }
   label$4 : {
    $2 = HEAP32[$0 >> 2];
    label$5 : {
     label$6 : {
      if (!($2 & 4)) {
       $2 = $2 >>> 5 | 0;
       if (($2 | 0) == 134217727) {
        break label$5
       }
       $6 = idr_find($2);
       if ($6) {
        break label$6
       }
       break label$5;
      }
      $6 = HEAP32[($2 & -256) >> 2];
      if (!$6) {
       break label$5
      }
     }
     $2 = HEAP32[$0 >> 2];
     label$8 : {
      if (!($2 & 4)) {
       break label$8
      }
      $8 = $2 & -256;
      if (!$8) {
       break label$8
      }
      if (HEAP32[$8 >> 2] == ($6 | 0)) {
       break label$4
      }
     }
    }
    HEAP32[16394] = HEAP32[$1 >> 2];
    $3 = (HEAP32[$0 >> 2] & 20) == 16 ? -2 : -11;
    break label$1;
   }
   if ($2 & 2) {
    $3 = HEAP32[$0 >> 2];
    $9 = $3 & $3 << 29 >> 31 & -256;
    $2 = HEAP32[$9 >> 2];
    $10 = $2 + 20 | 0;
    if (($10 | 0) == HEAP32[$2 + 20 >> 2]) {
     HEAP32[$2 + 16 >> 2] = HEAP32[20745];
     $3 = HEAP32[$0 >> 2];
    }
    $1 = HEAP32[$0 + 4 >> 2];
    $4 = $0 + 8 | 0;
    $5 = HEAP32[$4 >> 2];
    HEAP32[$1 + 4 >> 2] = $5;
    HEAP32[$5 >> 2] = $1;
    $5 = $2 + 24 | 0;
    $2 = HEAP32[$5 >> 2];
    $11 = $0 + 4 | 0;
    HEAP32[$5 >> 2] = $11;
    HEAP32[$0 + 4 >> 2] = $10;
    HEAP32[$2 >> 2] = $11;
    HEAP32[$4 >> 2] = $2;
    if ($3 & 8) {
     while (1) {
      $2 = HEAP32[$1 >> 2];
      $4 = HEAP32[$1 + 4 >> 2];
      HEAP32[$2 + 4 >> 2] = $4;
      HEAP32[$4 >> 2] = $2;
      $4 = HEAP32[$5 >> 2];
      HEAP32[$5 >> 2] = $1;
      HEAP32[$4 >> 2] = $1;
      HEAP32[$1 >> 2] = $10;
      HEAP32[$1 + 4 >> 2] = $4;
      $4 = $1 + -4 | 0;
      $1 = $2;
      if (HEAPU8[$4 | 0] & 8) {
       continue
      }
      break;
     }
    }
    $2 = $3 & -3;
    HEAP32[$0 >> 2] = $2;
    HEAP32[$9 + 80 >> 2] = HEAP32[$9 + 80 >> 2] + 1;
   }
   $1 = $0 + 8 | 0;
   $3 = HEAP32[$1 >> 2];
   $5 = HEAP32[$0 + 4 >> 2];
   HEAP32[$3 >> 2] = $5;
   HEAP32[$5 + 4 >> 2] = $3;
   $3 = $0 + 4 | 0;
   HEAP32[$0 + 4 >> 2] = $3;
   HEAP32[$1 >> 2] = $3;
   pwq_dec_nr_in_flight($8, $2 >>> 4 & 15);
   HEAP32[$0 >> 2] = HEAP32[$6 + 8 >> 2] << 5 | 1;
   $3 = 1;
  }
  $0 = $3;
  global$0 = $7 + 16 | 0;
  return $0;
 }
 
 function pwq_dec_nr_in_flight($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  label$1 : {
   if (($1 | 0) == 15) {
    break label$1
   }
   $9 = (($1 << 2) + $0 | 0) + 20 | 0;
   HEAP32[$9 >> 2] = HEAP32[$9 >> 2] + -1;
   $2 = HEAP32[$0 + 80 >> 2];
   HEAP32[$0 + 80 >> 2] = $2 + -1;
   $5 = HEAP32[$0 + 88 >> 2];
   label$2 : {
    if (($5 | 0) == ($0 + 88 | 0)) {
     break label$2
    }
    if (($2 | 0) > HEAP32[$0 + 84 >> 2]) {
     break label$2
    }
    $10 = $5 + -4 | 0;
    $6 = HEAP32[$10 >> 2];
    $11 = $6 & $6 << 29 >> 31 & -256;
    $4 = HEAP32[$11 >> 2];
    $12 = $4 + 20 | 0;
    if (($12 | 0) == HEAP32[$4 + 20 >> 2]) {
     HEAP32[$4 + 16 >> 2] = HEAP32[20745];
     $6 = HEAP32[$10 >> 2];
    }
    $3 = HEAP32[$5 >> 2];
    $2 = HEAP32[$5 + 4 >> 2];
    HEAP32[$3 + 4 >> 2] = $2;
    HEAP32[$2 >> 2] = $3;
    $8 = $4 + 24 | 0;
    $2 = HEAP32[$8 >> 2];
    HEAP32[$8 >> 2] = $5;
    HEAP32[$5 >> 2] = $12;
    HEAP32[$2 >> 2] = $5;
    HEAP32[$5 + 4 >> 2] = $2;
    if ($6 & 8) {
     while (1) {
      $2 = HEAP32[$3 >> 2];
      $4 = HEAP32[$3 + 4 >> 2];
      HEAP32[$2 + 4 >> 2] = $4;
      HEAP32[$4 >> 2] = $2;
      $4 = HEAP32[$8 >> 2];
      HEAP32[$8 >> 2] = $3;
      HEAP32[$4 >> 2] = $3;
      HEAP32[$3 >> 2] = $12;
      HEAP32[$3 + 4 >> 2] = $4;
      $4 = $3 + -4 | 0;
      $3 = $2;
      if (HEAPU8[$4 | 0] & 8) {
       continue
      }
      break;
     }
    }
    HEAP32[$10 >> 2] = $6 & -3;
    HEAP32[$11 + 80 >> 2] = HEAP32[$11 + 80 >> 2] + 1;
   }
   if (HEAP32[$0 + 12 >> 2] != ($1 | 0)) {
    break label$1
   }
   if (HEAP32[$9 >> 2]) {
    break label$1
   }
   HEAP32[$0 + 12 >> 2] = -1;
   $2 = HEAP32[$0 + 4 >> 2];
   $1 = HEAP32[$2 + 36 >> 2] + -1 | 0;
   HEAP32[$2 + 36 >> 2] = $1;
   if ($1) {
    break label$1
   }
   complete(HEAP32[$2 + 40 >> 2] + 12 | 0);
  }
  $1 = HEAP32[$0 + 16 >> 2] + -1 | 0;
  HEAP32[$0 + 16 >> 2] = $1;
  label$6 : {
   if ($1) {
    break label$6
   }
   if (!(HEAPU8[HEAP32[$0 + 4 >> 2] + 128 | 0] & 2)) {
    break label$6
   }
   $2 = HEAP32[16394];
   HEAP32[16394] = 0;
   $1 = HEAP32[16393];
   $3 = HEAP32[$0 + 112 >> 2];
   HEAP32[$7 + 12 >> 2] = $3;
   label$7 : {
    if (HEAP32[$7 + 12 >> 2] & 1) {
     break label$7
    }
    $0 = $0 + 112 | 0;
    HEAP32[$0 >> 2] = $3 | 1;
    if ($3 & 1) {
     break label$7
    }
    __queue_work(1, $1, $0);
   }
   HEAP32[16394] = $2;
  }
  global$0 = $7 + 16 | 0;
 }
 
 function __flush_work() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  label$1 : {
   if (!HEAPU8[65565]) {
    break label$1
   }
   HEAP32[16394] = 0;
   label$2 : {
    $0 = HEAP32[4222];
    label$3 : {
     if (!($0 & 4)) {
      $0 = $0 >>> 5 | 0;
      if (($0 | 0) == 134217727) {
       break label$2
      }
      $0 = idr_find($0);
      if ($0) {
       break label$3
      }
      break label$2;
     }
     $0 = HEAP32[($0 & -256) >> 2];
     if (!$0) {
      break label$2
     }
    }
    label$5 : {
     label$6 : {
      $1 = HEAP32[4222];
      $3 = $1 << 29 >> 31 & $1 & -256;
      label$7 : {
       if ($3) {
        $1 = 0;
        if (HEAP32[$3 >> 2] != ($0 | 0)) {
         break label$7
        }
        break label$5;
       }
       $1 = HEAP32[$0 + 248 >> 2];
       if (!$1) {
        break label$7
       }
       while (1) {
        if (HEAP32[$1 + 12 >> 2] == HEAP32[4225] ? HEAP32[$1 + 8 >> 2] == 16888 : 0) {
         break label$6
        }
        $1 = HEAP32[$1 >> 2];
        if ($1) {
         continue
        }
        break;
       };
      }
      HEAP32[16394] = 1;
      break label$1;
     }
     $3 = HEAP32[$1 + 16 >> 2];
    }
    label$11 : {
     if (HEAPU8[HEAP32[$3 + 4 >> 2] + 128 | 0] & 8) {
      break label$11
     }
     if (HEAP32[1] & 2031872) {
      break label$11
     }
     if (!(HEAPU8[HEAP32[2] + 12 | 0] & 32)) {
      break label$11
     }
    }
    $4 = $2 | 4;
    HEAP32[$2 + 8 >> 2] = $4;
    HEAP32[$2 + 12 >> 2] = 6;
    HEAP32[$2 >> 2] = -31;
    HEAP32[$2 + 4 >> 2] = $4;
    HEAP32[$2 + 16 >> 2] = 0;
    $0 = $2 + 20 | 0;
    HEAP32[$0 + 4 >> 2] = $0;
    HEAP32[$0 >> 2] = $0;
    HEAP32[$2 + 28 >> 2] = HEAP32[2];
    label$12 : {
     if ($1) {
      $0 = HEAP32[$1 + 20 >> 2];
      $5 = 240;
      break label$12;
     }
     $1 = HEAP32[4222];
     HEAP32[4222] = $1 | 8;
     $0 = HEAP32[4223];
     $5 = $1 & 8 | 240;
    }
    $1 = HEAP32[$0 + 4 >> 2];
    HEAP32[$0 + 4 >> 2] = $4;
    HEAP32[$2 >> 2] = $3 | $5 | 5;
    HEAP32[$1 >> 2] = $4;
    HEAP32[$2 + 8 >> 2] = $1;
    HEAP32[$3 + 16 >> 2] = HEAP32[$3 + 16 >> 2] + 1;
    HEAP32[$2 + 4 >> 2] = $0;
    $0 = HEAP32[$3 >> 2];
    label$14 : {
     if (HEAP32[$0 + 380 >> 2]) {
      break label$14
     }
     $1 = HEAP32[$0 + 36 >> 2];
     if (!$1) {
      break label$14
     }
     if (($1 | 0) == ($0 + 36 | 0)) {
      break label$14
     }
     wake_up_process(HEAP32[$1 + 28 >> 2]);
    }
    HEAP32[16394] = 1;
    wait_for_completion($2 + 16 | 0);
    break label$1;
   }
   HEAP32[16394] = 1;
  }
  global$0 = $2 + 32 | 0;
 }
 
 function wq_barrier_func($0) {
  $0 = $0 | 0;
  complete($0 + 16 | 0);
 }
 
 function print_worker_info($0) {
  var $1 = 0, $2 = 0;
  $1 = global$0 - 112 | 0;
  global$0 = $1;
  HEAP32[$1 + 108 >> 2] = 0;
  $2 = $1 + 96 | 0;
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  HEAP32[$1 + 88 >> 2] = 0;
  HEAP32[$1 + 92 >> 2] = 0;
  HEAP32[$1 + 80 >> 2] = 0;
  HEAP32[$1 + 84 >> 2] = 0;
  $2 = $1 - -64 | 0;
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  HEAP32[$1 + 56 >> 2] = 0;
  HEAP32[$1 + 60 >> 2] = 0;
  HEAP32[$1 + 48 >> 2] = 0;
  HEAP32[$1 + 52 >> 2] = 0;
  HEAP32[$1 + 44 >> 2] = 0;
  HEAP32[$1 + 40 >> 2] = 0;
  label$1 : {
   if (!(HEAPU8[$0 + 12 | 0] & 32)) {
    break label$1
   }
   $0 = kthread_probe_data($0);
   __probe_kernel_read($1 + 108 | 0, $0 + 12 | 0, 4);
   __probe_kernel_read($1 + 44 | 0, $0 + 16 | 0, 4);
   __probe_kernel_read($1 + 40 | 0, HEAP32[$1 + 44 >> 2] + 4 | 0, 4);
   __probe_kernel_read($1 + 80 | 0, HEAP32[$1 + 40 >> 2] + 88 | 0, 23);
   __probe_kernel_read($1 + 48 | 0, $0 + 56 | 0, 23);
   $0 = HEAP32[$1 + 108 >> 2];
   if (HEAPU8[$1 + 48 | 0] | HEAPU8[$1 + 80 | 0] ? 0 : !$0) {
    break label$1
   }
   HEAP32[$1 + 24 >> 2] = $0;
   HEAP32[$1 + 16 >> 2] = 32777;
   HEAP32[$1 + 20 >> 2] = $1 + 80;
   printk(20885, $1 + 16 | 0);
   if (strcmp($1 + 80 | 0, $1 + 48 | 0)) {
    HEAP32[$1 >> 2] = $1 + 48;
    printk(20905, $1);
   }
   printk(20913, 0);
  }
  global$0 = $1 + 112 | 0;
 }
 
 function __send_signal($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  label$1 : {
   if (!prepare_signal($0, $2, ($1 | 0) == 1)) {
    break label$1
   }
   $5 = $3 ? HEAP32[$2 + 732 >> 2] + 32 | 0 : $2 + 752 | 0;
   label$4 : {
    if (($0 | 0) <= 31) {
     $4 = 1 << $0 + -1;
     if ($4 & HEAP32[$5 + 8 >> 2]) {
      break label$1
     }
     if ($4 & 262400) {
      break label$4
     }
    }
    if (HEAPU8[$2 + 14 | 0] & 32) {
     break label$4
    }
    label$7 : {
     label$8 : {
      $4 = $0;
      $8 = $2;
      $6 = 0;
      label$6 : {
       if (($0 | 0) > 31) {
        break label$6
       }
       $6 = 1;
       if ($1 >>> 0 < 2) {
        break label$6
       }
       $6 = (HEAP32[$1 + 8 >> 2] ^ -1) >>> 31 | 0;
      }
      $4 = __sigqueue_alloc($4, $8, $6);
      if ($4) {
       $7 = HEAP32[$5 + 4 >> 2];
       HEAP32[$5 + 4 >> 2] = $4;
       HEAP32[$4 >> 2] = $5;
       HEAP32[$7 >> 2] = $4;
       HEAP32[$4 + 4 >> 2] = $7;
       if (($1 | 0) == 1) {
        break label$8
       }
       if ($1) {
        break label$7
       }
       memset($4 + 12 | 0, 0, 32);
       $1 = $4 + 16 | 0;
       HEAP32[$1 >> 2] = 0;
       HEAP32[$1 + 4 >> 2] = 0;
       HEAP32[$4 + 12 >> 2] = $0;
       $8 = HEAP32[2];
       $1 = HEAP32[$2 + 548 >> 2];
       __inlined_func$task_active_pid_ns : {
        if ($1) {
         $1 = HEAP32[($1 + (HEAP32[$1 + 4 >> 2] << 3) | 0) + 36 >> 2];
         break __inlined_func$task_active_pid_ns;
        }
        $1 = 0;
       }
       $1 = __task_pid_nr_ns($8, 1, $1);
       HEAP32[$4 + 28 >> 2] = 0;
       HEAP32[$4 + 24 >> 2] = $1;
       break label$4;
      }
      if (($0 | 0) < 32) {
       break label$4
      }
      if ($1 >>> 0 < 2) {
       break label$4
      }
      if (HEAP32[$1 + 8 >> 2]) {
       break label$1
      }
      break label$4;
     }
     memset($4 + 12 | 0, 0, 32);
     $1 = $4 + 24 | 0;
     HEAP32[$1 >> 2] = 0;
     HEAP32[$1 + 4 >> 2] = 0;
     $1 = $4 + 16 | 0;
     HEAP32[$1 >> 2] = 0;
     HEAP32[$1 + 4 >> 2] = 128;
     HEAP32[$4 + 12 >> 2] = $0;
     break label$4;
    }
    memcpy($4 + 12 | 0, $1, 32);
   }
   $7 = 1 << $0 + -1;
   HEAP32[$5 + 8 >> 2] = $7 | HEAP32[$5 + 8 >> 2];
   label$10 : {
    if ($3 >>> 0 < 2) {
     break label$10
    }
    $1 = HEAP32[HEAP32[$2 + 732 >> 2] + 44 >> 2];
    if (!$1) {
     break label$10
    }
    $4 = $1 + -4 | 0;
    if (!$4) {
     break label$10
    }
    $5 = !($7 & 3932160) | ($0 | 0) > 31;
    $6 = ($0 | 0) != 18;
    while (1) {
     $1 = HEAP32[$4 >> 2];
     $8 = $4;
     label$12 : {
      if ($6) {
       if ($5) {
        break label$12
       }
       $1 = $1 & -131073;
      } else {
       $1 = $1 & -3932161
      }
      HEAP32[$8 >> 2] = $1;
     }
     HEAP32[$4 >> 2] = $1 | $7;
     $1 = HEAP32[$4 + 4 >> 2];
     $4 = $1 ? $1 + -4 | 0 : 0;
     if ($4) {
      continue
     }
     break;
    };
   }
   complete_signal($0, $2, $3);
  }
 }
 
 function do_send_sig_info($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0;
  label$1 : {
   $4 = HEAP32[$2 + 736 >> 2];
   if (!$4) {
    break label$1
   }
   $5 = $2 + 736 | 0;
   while (1) {
    $6 = HEAP32[16398];
    HEAP32[16398] = 0;
    if (HEAP32[$5 >> 2] == ($4 | 0)) {
     break label$1
    }
    HEAP32[16398] = $6;
    $4 = HEAP32[$5 >> 2];
    if ($4) {
     continue
    }
    break;
   };
   return;
  }
  if ($4) {
   __send_signal($0, $1, $2, $3);
   HEAP32[16398] = $6;
  }
 }
 
 function prepare_signal($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0, $35 = 0, $36 = 0, $37 = 0, $38 = 0, $39 = 0, $40 = 0, $41 = 0, $42 = 0;
  $6 = HEAP32[$1 + 732 >> 2];
  $3 = HEAP32[$6 + 64 >> 2];
  label$1 : {
   if ($3 & 12) {
    if ($3 & 4) {
     break label$1
    }
    return ($0 | 0) == 9;
   }
   if (($0 | 0) > 31) {
    break label$1
   }
   label$3 : {
    label$4 : {
     if (1 << $0 + -1 & 3932160) {
      $3 = $6 + 40 | 0;
      $4 = HEAP32[$3 >> 2];
      label$6 : {
       if (!($4 & 131072)) {
        break label$6
       }
       HEAP32[$3 >> 2] = $4 & -131073;
       $5 = $6 + 32 | 0;
       $3 = HEAP32[$5 >> 2];
       if (($3 | 0) == ($5 | 0)) {
        break label$6
       }
       while (1) {
        $6 = $3;
        $4 = $3;
        $3 = HEAP32[$3 >> 2];
        label$8 : {
         if (HEAP32[$4 + 12 >> 2] != 18) {
          break label$8
         }
         $9 = HEAP32[$4 + 4 >> 2];
         HEAP32[$3 + 4 >> 2] = $9;
         HEAP32[$9 >> 2] = $3;
         HEAP32[$4 + 4 >> 2] = $6;
         HEAP32[$4 >> 2] = $4;
         if (HEAP8[$4 + 8 | 0] & 1) {
          break label$8
         }
         $6 = HEAP32[$4 + 44 >> 2];
         HEAP32[$6 + 8 >> 2] = HEAP32[$6 + 8 >> 2] + -1;
         free_uid($6);
         kmem_cache_free(HEAP32[16397], $4);
        }
        if (($3 | 0) != ($5 | 0)) {
         continue
        }
        break;
       };
       $6 = HEAP32[$1 + 732 >> 2];
      }
      $15 = 12;
      $9 = HEAP32[$6 + 12 >> 2];
      if (($9 | 0) == ($6 + 12 | 0)) {
       break label$1
      }
      $16 = 160;
      $17 = 131072;
      $18 = $1 + 732 | 0;
      $19 = -131073;
      $20 = 168;
      $21 = 18;
      $22 = 1;
      $23 = -1;
      $3 = 0;
      break label$4;
     }
     if (($0 | 0) != 18) {
      break label$1
     }
     $3 = $6;
     $4 = $3 + 40 | 0;
     $5 = HEAP32[$4 >> 2];
     label$9 : {
      if (!($5 & 3932160)) {
       break label$9
      }
      HEAP32[$4 >> 2] = $5 & -3932161;
      $7 = $6 + 32 | 0;
      $4 = HEAP32[$7 >> 2];
      if (($4 | 0) == ($7 | 0)) {
       break label$9
      }
      while (1) {
       $3 = $4;
       $5 = $3;
       $4 = HEAP32[$3 >> 2];
       label$11 : {
        if (!(1 << HEAP32[$3 + 12 >> 2] + -1 & 3932160)) {
         break label$11
        }
        $8 = HEAP32[$5 + 4 >> 2];
        HEAP32[$4 + 4 >> 2] = $8;
        HEAP32[$8 >> 2] = $4;
        HEAP32[$5 + 4 >> 2] = $3;
        HEAP32[$5 >> 2] = $5;
        if (HEAP8[$5 + 8 | 0] & 1) {
         break label$11
        }
        $3 = HEAP32[$5 + 44 >> 2];
        HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] + -1;
        free_uid($3);
        kmem_cache_free(HEAP32[16397], $5);
       }
       if (($4 | 0) != ($7 | 0)) {
        continue
       }
       break;
      };
      $3 = HEAP32[$1 + 732 >> 2];
     }
     $24 = 12;
     $7 = HEAP32[$3 + 12 >> 2];
     if (($7 | 0) == ($3 + 12 | 0)) {
      break label$3
     }
     $25 = 160;
     $26 = -592;
     $12 = 3932160;
     $27 = -164;
     $28 = -458753;
     $29 = 3670016;
     $30 = 2097152;
     $31 = -2555905;
     $32 = 21;
     $33 = -574;
     $8 = 1;
     $11 = 4;
     $34 = $1 + 732 | 0;
     $35 = -588;
     $36 = -580;
     $37 = 1048576;
     $38 = 4194304;
     $39 = 9;
     $40 = 169;
     $41 = -3932161;
     $42 = 168;
     $13 = -1;
     $3 = 1;
    }
    while (1) {
     label$13 : {
      label$14 : {
       if (!$3) {
        $3 = HEAP32[$9 + 168 >> 2];
        label$17 : {
         if (!($3 & $17)) {
          break label$17
         }
         HEAP32[$9 + $20 >> 2] = $3 & $19;
         $5 = $9 + $16 | 0;
         $3 = HEAP32[$5 >> 2];
         if (($3 | 0) == ($5 | 0)) {
          break label$17
         }
         while (1) {
          $6 = $3;
          $4 = $3;
          $3 = HEAP32[$3 >> 2];
          label$19 : {
           if (HEAP32[$4 + 12 >> 2] != ($21 | 0)) {
            break label$19
           }
           $10 = HEAP32[$4 + 4 >> 2];
           HEAP32[$3 + 4 >> 2] = $10;
           HEAP32[$10 >> 2] = $3;
           HEAP32[$4 + 4 >> 2] = $6;
           HEAP32[$4 >> 2] = $4;
           if (HEAPU8[$4 + 8 | 0] & $22) {
            break label$19
           }
           $6 = HEAP32[$4 + 44 >> 2];
           HEAP32[$6 + 8 >> 2] = HEAP32[$6 + 8 >> 2] + $23;
           free_uid($6);
           kmem_cache_free(HEAP32[16397], $4);
          }
          if (($3 | 0) != ($5 | 0)) {
           continue
          }
          break;
         };
        }
        $9 = HEAP32[$9 >> 2];
        $6 = HEAP32[$18 >> 2];
        if (($9 | 0) != ($15 + $6 | 0)) {
         break label$14
        }
        break label$1;
       }
       $3 = HEAP32[$7 + 168 >> 2];
       label$20 : {
        if (!($3 & $12)) {
         break label$20
        }
        HEAP32[$7 + $42 >> 2] = $3 & $41;
        $10 = $7 + $25 | 0;
        $4 = HEAP32[$10 >> 2];
        if (($4 | 0) == ($10 | 0)) {
         break label$20
        }
        while (1) {
         $3 = $4;
         $5 = $3;
         $4 = HEAP32[$3 >> 2];
         label$22 : {
          if (!($8 << HEAP32[$3 + 12 >> 2] + $13 & $12)) {
           break label$22
          }
          $14 = HEAP32[$5 + 4 >> 2];
          HEAP32[$4 + 4 >> 2] = $14;
          HEAP32[$14 >> 2] = $4;
          HEAP32[$5 + 4 >> 2] = $3;
          HEAP32[$5 >> 2] = $5;
          if (HEAPU8[$5 + 8 | 0] & $8) {
           break label$22
          }
          $3 = HEAP32[$5 + 44 >> 2];
          HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] + $13;
          free_uid($3);
          kmem_cache_free(HEAP32[16397], $5);
         }
         if (($4 | 0) != ($10 | 0)) {
          continue
         }
         break;
        };
       }
       $3 = $7 + $27 | 0;
       $4 = HEAP32[$3 >> 2];
       HEAP32[$3 >> 2] = $4 & $28;
       if (($30 | 0) == ($4 & $29)) {
        HEAP32[$3 >> 2] = $4 & $31;
        wake_up_bit($3, $32);
       }
       $4 = $7 + $26 | 0;
       label$24 : {
        if (HEAPU8[$7 + $33 | 0] & $8) {
         $5 = HEAP32[$7 + $35 >> 2];
         $10 = HEAP32[$5 >> 2];
         label$26 : {
          if (HEAPU8[$7 + $40 | 0] & $8 ? $10 & $11 : 0) {
           break label$26
          }
          if (HEAPU8[$7 + $36 | 0] & $11) {
           break label$26
          }
          HEAP32[$3 >> 2] = HEAP32[$3 >> 2] | $37;
         }
         HEAP32[$5 >> 2] = $10 | $11;
         wake_up_state($4, HEAP32[$3 >> 2] & $38 ? $39 : $8);
         break label$24;
        }
        wake_up_state($4, $11);
       }
       $7 = HEAP32[$7 >> 2];
       $3 = HEAP32[$34 >> 2];
       if (($7 | 0) != ($24 + $3 | 0)) {
        break label$13
       }
       break label$3;
      }
      $3 = 0;
      continue;
     }
     $3 = 1;
     continue;
    };
   }
   $4 = 34;
   $5 = $6 - -64 | 0;
   $7 = HEAP32[$5 >> 2];
   label$28 : {
    if (!($7 & 1)) {
     $4 = 18;
     if (!HEAP32[$6 + 60 >> 2]) {
      break label$28
     }
    }
    HEAP32[$6 + 60 >> 2] = 0;
    HEAP32[$6 + 48 >> 2] = 0;
    HEAP32[$5 >> 2] = $7 & -52 | $4;
   }
   $6 = $3;
  }
  $4 = $0 + -1 | 0;
  $3 = 1 << $4;
  if ($3 & HEAP32[$1 + 740 >> 2]) {
   return 1
  }
  if ($3 & HEAP32[$1 + 744 >> 2]) {
   return 1
  }
  label$32 : {
   if (($0 | 0) == 9) {
    break label$32
   }
   if (!HEAP32[$1 + 16 >> 2]) {
    break label$32
   }
   return 1;
  }
  $4 = HEAP32[(HEAP32[$1 + 736 >> 2] + Math_imul($4, 12) | 0) + 4 >> 2];
  $8 = 0;
  label$33 : {
   $5 = $3 & 262400;
   label$34 : {
    label$35 : {
     if (!$5) {
      break label$35
     }
     if (($0 | 0) > 31) {
      break label$35
     }
     if (HEAP32[$1 + 500 >> 2] == 1) {
      break label$34
     }
    }
    label$36 : {
     label$37 : {
      if ($4) {
       break label$37
      }
      if (!(HEAP32[$6 + 64 >> 2] & 64)) {
       break label$37
      }
      if (!$5) {
       break label$34
      }
      if (($0 | 0) > 31) {
       break label$34
      }
      if ($2) {
       break label$36
      }
      break label$34;
     }
     if (($4 | 0) == 1) {
      break label$34
     }
    }
    if (($0 | 0) > 31) {
     break label$33
    }
    $8 = 1;
    if ($4) {
     break label$34
    }
    $8 = !($3 & 138608640);
   }
   return $8;
  }
  return 1;
 }
 
 function __sigqueue_alloc($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  $3 = HEAP32[HEAP32[$1 + 692 >> 2] + 80 >> 2];
  HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + 1;
  HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] + 1;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      if ($2) {
       break label$4
      }
      if (HEAPU32[$3 + 8 >> 2] <= HEAPU32[HEAP32[$1 + 732 >> 2] + 320 >> 2]) {
       break label$4
      }
      if (!HEAP32[16396]) {
       break label$2
      }
      if (___ratelimit(3904, 21e3)) {
       break label$3
      }
      break label$2;
     }
     $1 = kmem_cache_alloc(HEAP32[16397], 4718624);
     if (!$1) {
      break label$2
     }
     HEAP32[$1 + 44 >> 2] = $3;
     HEAP32[$1 + 8 >> 2] = 0;
     HEAP32[$1 + 4 >> 2] = $1;
     HEAP32[$1 >> 2] = $1;
     break label$1;
    }
    $1 = HEAP32[2];
    $2 = HEAP32[$1 + 496 >> 2];
    HEAP32[$4 + 8 >> 2] = $0;
    HEAP32[$4 + 4 >> 2] = $2;
    HEAP32[$4 >> 2] = $1 + 700;
    printk(21021, $4);
   }
   $0 = $3 + 8 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
   free_uid($3);
   $1 = 0;
  }
  global$0 = $4 + 16 | 0;
  return $1;
 }
 
 function complete_signal($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $4 = HEAP32[$1 + 732 >> 2];
  label$1 : {
   label$2 : {
    $6 = $0 + -1 | 0;
    $5 = 1 << $6;
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        if ($5 & HEAP32[$1 + 740 >> 2]) {
         break label$6
        }
        if (HEAPU8[$1 + 12 | 0] & 4) {
         break label$6
        }
        $3 = $1;
        if (($0 | 0) == 9) {
         break label$4
        }
        if (HEAP32[$1 >> 2] & 12) {
         break label$6
        }
        if (HEAP32[3311] == ($1 | 0)) {
         break label$5
        }
        if (!(HEAP32[HEAP32[$1 + 4 >> 2] >> 2] & 4)) {
         break label$5
        }
       }
       if (!$2) {
        break label$1
       }
       if (HEAP32[$1 + 584 >> 2] == ($1 + 584 | 0)) {
        break label$1
       }
       $7 = ($0 | 0) == 9;
       $8 = $4 + 28 | 0;
       $2 = HEAP32[$4 + 28 >> 2];
       $3 = $2;
       while (1) {
        label$7 : {
         label$9 : {
          if ($5 & HEAP32[$3 + 740 >> 2]) {
           break label$9
          }
          if (HEAPU8[$3 + 12 | 0] & 4) {
           break label$9
          }
          if ($7) {
           break label$7
          }
          if (HEAP32[$3 >> 2] & 12) {
           break label$9
          }
          if (HEAP32[3311] == ($3 | 0)) {
           break label$7
          }
          if (!(HEAP32[HEAP32[$3 + 4 >> 2] >> 2] & 4)) {
           break label$7
          }
          $2 = HEAP32[$8 >> 2];
         }
         $3 = HEAP32[$3 + 584 >> 2] + -584 | 0;
         if (($3 | 0) != ($2 | 0)) {
          continue
         }
         break label$1;
        }
        break;
       };
       HEAP32[$4 + 28 >> 2] = $3;
      }
      $2 = 1;
      if (($0 | 0) > 31) {
       break label$3
      }
     }
     if ($5 & 142540800) {
      break label$2
     }
     $2 = 0;
    }
    if (HEAP32[(HEAP32[$1 + 736 >> 2] + Math_imul($6, 12) | 0) + 4 >> 2]) {
     break label$2
    }
    if (HEAPU8[$4 + 64 | 0] & 4) {
     break label$2
    }
    if ($5 & HEAP32[$3 + 744 >> 2]) {
     break label$2
    }
    if (HEAP32[$1 + 16 >> 2] ? ($0 | 0) != 9 : 0) {
     break label$2
    }
    if (($2 ^ 1) & ($5 & 1098908924) != 0) {
     break label$2
    }
    HEAP32[$4 + 48 >> 2] = $0;
    HEAP32[$4 + 60 >> 2] = 0;
    HEAP32[$4 + 64 >> 2] = 4;
    $3 = $1;
    while (1) {
     $0 = HEAP32[$3 + 428 >> 2];
     HEAP32[$3 + 428 >> 2] = $0 & -2031617;
     if ($0 & 2097152) {
      $2 = $3 + 428 | 0;
      HEAP32[$2 >> 2] = $0 & -4128769;
      wake_up_bit($2, 21);
     }
     $0 = $3 + 760 | 0;
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 256;
     $0 = HEAP32[$3 + 4 >> 2];
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 4;
     wake_up_state($3, 257);
     $3 = HEAP32[$3 + 584 >> 2] + -584 | 0;
     if (($3 | 0) != ($1 | 0)) {
      continue
     }
     break;
    };
    break label$1;
   }
   $1 = HEAP32[$3 + 4 >> 2];
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] | 4;
   wake_up_state($3, ($0 | 0) == 9 ? 257 : 1);
  }
 }
 
 function do_no_restart_syscall($0) {
  $0 = $0 | 0;
  return -4;
 }
 
 function free_uid($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  label$1 : {
   if (!$0) {
    break label$1
   }
   if (!refcount_dec_and_lock_irqsave($0, $2 + 12 | 0)) {
    break label$1
   }
   $4 = HEAP32[$2 + 12 >> 2];
   $1 = HEAP32[$0 + 28 >> 2];
   if ($1) {
    $3 = HEAP32[$0 + 24 >> 2];
    HEAP32[$1 >> 2] = $3;
    if ($3) {
     HEAP32[$3 + 4 >> 2] = $1
    }
    $1 = $0 + 24 | 0;
    HEAP32[$1 >> 2] = 0;
    HEAP32[$1 + 4 >> 2] = 0;
   }
   HEAP32[16400] = $4;
   kmem_cache_free(HEAP32[16399], $0);
  }
  global$0 = $2 + 16 | 0;
 }
 
 function raise_softirq() {
  var $0 = 0, $1 = 0;
  HEAP32[1088] = HEAP32[1088] | 512;
  $1 = HEAPU8[65656];
  HEAP8[65656] = 0;
  label$1 : {
   if (HEAP32[1] & 2096896) {
    break label$1
   }
   $0 = HEAP32[1120];
   if (!$0) {
    break label$1
   }
   if (!HEAP32[$0 >> 2]) {
    break label$1
   }
   wake_up_process($0);
  }
  HEAP8[65656] = $1 & 1;
 }
 
 function tasklet_action($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  HEAP32[1122] = 4484;
  HEAP8[65656] = 1;
  $0 = HEAP32[1121];
  HEAP32[1121] = 0;
  label$1 : {
   label$2 : {
    if (!$0) {
     break label$2
    }
    while (1) {
     $3 = HEAP32[$0 >> 2];
     if (HEAP32[$0 + 8 >> 2]) {
      HEAP32[$0 >> 2] = 0;
      HEAP32[HEAP32[1122] >> 2] = $0;
      HEAP32[1122] = $0;
      HEAP32[1088] = HEAP32[1088] | 64;
      HEAP8[65656] = 1;
      $0 = $3;
      if ($0) {
       continue
      }
      break label$2;
     }
     $2 = HEAP32[$0 + 4 >> 2];
     HEAP32[$1 + 12 >> 2] = $2;
     if (!(HEAP32[$1 + 12 >> 2] & 1)) {
      break label$1
     }
     HEAP32[$0 + 4 >> 2] = $2 & -2;
     if (!($2 & 1)) {
      break label$1
     }
     FUNCTION_TABLE[HEAP32[$0 + 12 >> 2]](HEAP32[$0 + 16 >> 2]);
     $0 = $3;
     if ($0) {
      continue
     }
     break;
    };
   }
   global$0 = $1 + 16 | 0;
   return;
  }
  HEAP32[$1 + 8 >> 2] = 21124;
  HEAP32[$1 + 4 >> 2] = 522;
  HEAP32[$1 >> 2] = 21114;
  printk(21076, $1);
  abort();
 }
 
 function tasklet_hi_action($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  HEAP32[1124] = 4492;
  HEAP8[65656] = 1;
  $0 = HEAP32[1123];
  HEAP32[1123] = 0;
  label$1 : {
   label$2 : {
    if (!$0) {
     break label$2
    }
    while (1) {
     $3 = HEAP32[$0 >> 2];
     if (HEAP32[$0 + 8 >> 2]) {
      HEAP32[$0 >> 2] = 0;
      HEAP32[HEAP32[1124] >> 2] = $0;
      HEAP32[1124] = $0;
      HEAP32[1088] = HEAP32[1088] | 1;
      HEAP8[65656] = 1;
      $0 = $3;
      if ($0) {
       continue
      }
      break label$2;
     }
     $2 = HEAP32[$0 + 4 >> 2];
     HEAP32[$1 + 12 >> 2] = $2;
     if (!(HEAP32[$1 + 12 >> 2] & 1)) {
      break label$1
     }
     HEAP32[$0 + 4 >> 2] = $2 & -2;
     if (!($2 & 1)) {
      break label$1
     }
     FUNCTION_TABLE[HEAP32[$0 + 12 >> 2]](HEAP32[$0 + 16 >> 2]);
     $0 = $3;
     if ($0) {
      continue
     }
     break;
    };
   }
   global$0 = $1 + 16 | 0;
   return;
  }
  HEAP32[$1 + 8 >> 2] = 21124;
  HEAP32[$1 + 4 >> 2] = 522;
  HEAP32[$1 >> 2] = 21114;
  printk(21076, $1);
  abort();
 }
 
 function __cpuhp_setup_state_cpuslocked() {
  var $0 = 0, $1 = 0, $2 = 0;
  __inlined_func$mutex_lock : {
   if (HEAP32[1125]) {
    __mutex_lock();
    break __inlined_func$mutex_lock;
   }
   HEAP32[1125] = HEAP32[2];
  }
  $0 = 29;
  label$1 : {
   if (($0 | 0) == 64) {
    $1 = 5792;
    label$3 : {
     while (1) {
      if (!HEAP32[$1 >> 2]) {
       break label$3
      }
      $1 = $1 + 20 | 0;
      $2 = ($0 | 0) < 84;
      $0 = $0 + 1 | 0;
      if ($2) {
       continue
      }
      break;
     };
     break label$1;
    }
    if (($0 | 0) < 0) {
     break label$1
    }
   }
   $0 = Math_imul($0, 20);
   $1 = $0 + 4512 | 0;
   if (HEAP32[$1 >> 2]) {
    break label$1
   }
   HEAP32[$1 >> 2] = 27758;
   HEAP32[$0 + 4520 >> 2] = 89;
   HEAP32[$0 + 4516 >> 2] = 0;
   HEAP8[$0 + 4529 | 0] = 0;
   HEAP32[$0 + 4524 >> 2] = 0;
  }
  mutex_unlock(4500);
 }
 
 function boot_cpu_init() {
  HEAP32[16417] = HEAP32[16417] | 1;
  HEAP32[16418] = HEAP32[16418] | 1;
  HEAP32[16415] = HEAP32[16415] | 1;
  HEAP32[16416] = HEAP32[16416] | 1;
 }
 
 function panic($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $4 = global$0 - 48 | 0;
  global$0 = $4;
  HEAP8[66772] = 0;
  $2 = HEAPU8[66720];
  label$1 : {
   $3 = HEAP32[2125];
   if (($3 | 0) == -1) {
    HEAP8[66772] = 0;
    HEAP32[2125] = 0;
    break label$1;
   }
   HEAP8[66772] = 0;
   if (!$3) {
    break label$1
   }
   while (1) continue;
  }
  if (HEAP32[2968]) {
   HEAP32[2968] = 15
  }
  bust_spinlocks(1);
  HEAP32[$4 + 44 >> 2] = $1;
  $0 = vscnprintf(65696, 1024, $0, $1);
  label$4 : {
   if (!$0) {
    break label$4
   }
   if (HEAPU8[$0 + 65695 | 0] != 10) {
    break label$4
   }
   HEAP8[$0 + 65695 | 0] = 0;
  }
  HEAP32[$4 + 32 >> 2] = 65696;
  printk(21215, $4 + 32 | 0);
  label$5 : {
   if (!$2) {
    __printk_safe_flush(11948);
    break label$5;
   }
   if (!HEAPU8[65680]) {
    HEAP8[65680] = 1
   }
  }
  $3 = 0;
  atomic_notifier_call_chain(65676, 0, 65696);
  __printk_safe_flush(11948);
  kmsg_dump(1);
  bust_spinlocks(0);
  debug_locks_off();
  console_flush_on_panic();
  if (!HEAP32[16681]) {
   HEAP32[16681] = 9
  }
  label$9 : {
   label$102 : {
    $2 = HEAP32[2124];
    if (($2 | 0) < 1) {
     break label$102
    }
    HEAP32[$4 + 16 >> 2] = $2;
    printk(21249, $4 + 16 | 0);
    $2 = HEAP32[2124];
    if (($2 | 0) < 1) {
     break label$102
    }
    $1 = 0;
    $0 = 0;
    while (1) {
     if (($1 | 0) >= ($0 | 0)) {
      $3 = $3 ^ 1;
      $1 = (FUNCTION_TABLE[HEAP32[16681]]($3) | 0) + $1 | 0;
      $0 = $1 + 200 | 0;
      $2 = HEAP32[2124];
     }
     $1 = $1 + 100 | 0;
     if (($1 | 0) < (Math_imul($2, 1e3) | 0)) {
      continue
     }
     break;
    };
    break label$9;
   }
   $0 = 0;
  }
  if ($2) {
   kmsg_dump(3);
   machine_restart();
  }
  HEAP32[$4 >> 2] = 65696;
  printk(21278, $4);
  HEAP8[66772] = 1;
  $1 = 0;
  while (1) {
   if (($1 | 0) >= ($0 | 0)) {
    $3 = $3 ^ 1;
    $1 = (FUNCTION_TABLE[HEAP32[16681]]($3) | 0) + $1 | 0;
    $0 = $1 + 200 | 0;
   }
   $1 = $1 + 100 | 0;
   continue;
  };
 }
 
 function no_blink($0) {
  $0 = $0 | 0;
  return 0;
 }
 
 function print_tainted() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0;
  if (HEAP32[16691]) {
   $2 = sprintf(66736, 21382, 0);
   $3 = $2 + 66736 | 0;
   $1 = 21328;
   while (1) {
    HEAP8[$0 + $3 | 0] = HEAPU8[!(HEAP32[($0 >>> 3 & 28) + 66764 >> 2] & 1 << $0) + $1 | 0];
    $1 = $1 + 3 | 0;
    $0 = $0 + 1 | 0;
    if (($0 | 0) != 18) {
     continue
    }
    break;
   };
   HEAP8[($0 + $2 | 0) + 66736 | 0] = 0;
   return 66736;
  }
  snprintf(66736, 28, 21392, 0);
  return 66736;
 }
 
 function add_taint($0, $1) {
  label$1 : {
   if (($1 | 0) != 1) {
    break label$1
   }
   $1 = HEAP32[4943];
   HEAP32[4943] = 0;
   if (!$1) {
    break label$1
   }
   printk(21404, 0);
  }
  $1 = ($0 >>> 3 & 536870908) + 66764 | 0;
  HEAP32[$1 >> 2] = HEAP32[$1 >> 2] | 1 << ($0 & 31);
 }
 
 function arch_release_task_struct($0) {
  
 }
 
 function free_task($0) {
  var $1 = 0;
  if (HEAP32[$0 >> 2] == 128) {
   HEAP32[20665] = HEAP32[20665] + -64;
   $1 = Math_imul(HEAP32[HEAP32[20646] + Math_imul(HEAP32[$0 + 4 >> 2] >>> 16 | 0, 36) >> 2] >>> 31 | 0, 516) + 141776 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -64;
   __free_pages(HEAP32[20646] + Math_imul(HEAPU16[$0 + 6 >> 1], 36) | 0, 0);
   HEAP32[$0 + 4 >> 2] = 0;
  }
  if (HEAPU8[$0 + 14 | 0] & 32) {
   kfree(HEAP32[$0 + 604 >> 2])
  }
  kmem_cache_free(HEAP32[16695], $0);
 }
 
 function __mmdrop($0) {
  var $1 = 0, $2 = 0;
  $1 = global$0 - 96 | 0;
  global$0 = $1;
  label$1 : {
   if (($0 | 0) != 15736) {
    free_pages(HEAP32[$0 + 36 >> 2], 0);
    HEAP32[$0 + 360 >> 2] = 0;
    $2 = HEAP32[$0 + 340 >> 2];
    if (!$2) {
     break label$1
    }
    HEAP32[$1 + 88 >> 2] = $2;
    HEAP32[$1 + 84 >> 2] = 0;
    HEAP32[$1 + 80 >> 2] = $0;
    printk(21499, $1 + 80 | 0);
    break label$1;
   }
   HEAP32[$1 + 8 >> 2] = 21490;
   HEAP32[$1 + 4 >> 2] = 668;
   HEAP32[$1 >> 2] = 21483;
   printk(21452, $1);
   abort();
  }
  $2 = HEAP32[$0 + 344 >> 2];
  if ($2) {
   HEAP32[$1 + 72 >> 2] = $2;
   HEAP32[$1 + 68 >> 2] = 1;
   HEAP32[$1 + 64 >> 2] = $0;
   printk(21499, $1 - -64 | 0);
  }
  $2 = HEAP32[$0 + 348 >> 2];
  if ($2) {
   HEAP32[$1 + 56 >> 2] = $2;
   HEAP32[$1 + 52 >> 2] = 2;
   HEAP32[$1 + 48 >> 2] = $0;
   printk(21499, $1 + 48 | 0);
  }
  $2 = HEAP32[$0 + 352 >> 2];
  label$5 : {
   label$6 : {
    if (!$2) {
     if (HEAP32[$0 + 48 >> 2]) {
      break label$6
     }
     break label$5;
    }
    HEAP32[$1 + 40 >> 2] = $2;
    HEAP32[$1 + 36 >> 2] = 3;
    HEAP32[$1 + 32 >> 2] = $0;
    printk(21499, $1 + 32 | 0);
    if (!HEAP32[$0 + 48 >> 2]) {
     break label$5
    }
   }
   HEAP32[$1 + 16 >> 2] = HEAP32[$0 + 48 >> 2];
   printk(21550, $1 + 16 | 0);
  }
  kmem_cache_free(HEAP32[16694], $0);
  global$0 = $1 + 96 | 0;
 }
 
 function __put_task_struct($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  exit_creds($0);
  $3 = HEAP32[$0 + 732 >> 2];
  $1 = HEAP32[$3 >> 2] + -1 | 0;
  HEAP32[$3 >> 2] = $1;
  if ($1) {
   free_task($0);
   return;
  }
  $1 = HEAP32[$3 + 368 >> 2];
  label$2 : {
   if (!$1) {
    break label$2
   }
   $2 = HEAP32[$1 + 44 >> 2] + -1 | 0;
   HEAP32[$1 + 44 >> 2] = $2;
   if ($2) {
    break label$2
   }
   HEAP32[$1 + 384 >> 2] = -32;
   HEAP32[$1 + 396 >> 2] = 10;
   $2 = $1 + 388 | 0;
   HEAP32[$1 + 392 >> 2] = $2;
   HEAP32[$2 >> 2] = $2;
   queue_work_on(1, HEAP32[16393], $1 + 384 | 0);
  }
  kmem_cache_free(HEAP32[16696], $3);
  free_task($0);
 }
 
 function mmdrop_async_fn($0) {
  $0 = $0 | 0;
  __mmdrop($0 + -384 | 0);
 }
 
 function ns_to_timespec64($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  label$1 : {
   if ($1 | $2) {
    $1 = div_s64_rem($1, $2, 1e9, $3 + 12 | 0);
    $2 = i64toi32_i32$HIGH_BITS;
    $4 = HEAP32[$3 + 12 >> 2];
    if (($4 | 0) > -1) {
     break label$1
    }
    $4 = $4 + 1e9 | 0;
    HEAP32[$3 + 12 >> 2] = $4;
    $2 = $2 + -1 | 0;
    $1 = $1 + -1 | 0;
    if ($1 >>> 0 < 4294967295) {
     $2 = $2 + 1 | 0
    }
    break label$1;
   }
   $1 = 0;
   $2 = 0;
  }
  HEAP32[$0 + 8 >> 2] = $4;
  HEAP32[$0 >> 2] = $1;
  HEAP32[$0 + 4 >> 2] = $2;
  global$0 = $3 + 16 | 0;
 }
 
 function set_normalized_timespec64($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0;
  $6 = global$0 - 16 | 0;
  HEAP32[$6 + 8 >> 2] = $3;
  HEAP32[$6 + 12 >> 2] = $4;
  if (($4 | 0) < 0) {
   $5 = 1
  } else {
   if (($4 | 0) <= 0) {
    $5 = $3 >>> 0 >= 1e9 ? 0 : 1
   } else {
    $5 = 0
   }
  }
  if (!$5) {
   while (1) {
    $4 = $4 + -1 | 0;
    $3 = $3 + -1e9 | 0;
    if ($3 >>> 0 < 3294967296) {
     $4 = $4 + 1 | 0
    }
    HEAP32[$6 + 8 >> 2] = $3;
    HEAP32[$6 + 12 >> 2] = $4;
    $1 = $1 + 1 | 0;
    if ($1 >>> 0 < 1) {
     $2 = $2 + 1 | 0
    }
    if (($4 | 0) > 0) {
     $5 = 1
    } else {
     if (($4 | 0) >= 0) {
      $5 = $3 >>> 0 <= 999999999 ? 0 : 1
     } else {
      $5 = 0
     }
    }
    if ($5) {
     continue
    }
    break;
   }
  }
  if (($4 | 0) > -1) {
   $5 = 1
  } else {
   if (($4 | 0) >= -1) {
    $5 = $3 >>> 0 <= 4294967295 ? 0 : 1
   } else {
    $5 = 0
   }
  }
  if (!$5) {
   while (1) {
    $3 = $3 + 1e9 | 0;
    if ($3 >>> 0 < 1e9) {
     $4 = $4 + 1 | 0
    }
    HEAP32[$6 + 8 >> 2] = $3;
    HEAP32[$6 + 12 >> 2] = $4;
    $2 = $2 + -1 | 0;
    $1 = $1 + -1 | 0;
    if ($1 >>> 0 < 4294967295) {
     $2 = $2 + 1 | 0
    }
    if (($4 | 0) < 0) {
     $5 = 1
    } else {
     if (($4 | 0) <= 0) {
      $5 = $3 >>> 0 >= 0 ? 0 : 1
     } else {
      $5 = 0
     }
    }
    if ($5) {
     continue
    }
    break;
   }
  }
  HEAP32[$0 + 8 >> 2] = $3;
  HEAP32[$0 >> 2] = $1;
  HEAP32[$0 + 4 >> 2] = $2;
 }
 
 function calc_wheel_index($0, $1) {
  var $2 = 0;
  $2 = $0 - $1 | 0;
  if ($2 >>> 0 <= 62) {
   return $0 + 1 & 63
  }
  if ($2 >>> 0 <= 503) {
   return $0 + 8 >>> 3 & 63 | 64
  }
  if ($2 >>> 0 <= 4031) {
   return $0 - -64 >>> 6 & 63 | 128
  }
  if ($2 >>> 0 <= 32255) {
   return $0 + 512 >>> 9 & 63 | 192
  }
  if ($2 >>> 0 <= 258047) {
   return $0 + 4096 >>> 12 & 63 | 256
  }
  if ($2 >>> 0 <= 2064383) {
   return $0 + 32768 >>> 15 & 63 | 320
  }
  if ($2 >>> 0 <= 16515071) {
   return $0 + 262144 >>> 18 & 63 | 384
  }
  if ($2 >>> 0 <= 132120575) {
   return $0 + 2097152 >>> 21 & 63 | 448
  }
  if (($2 | 0) > -1) {
   return $0 >>> 0 > 1056964607 ? 575 : $0 + 16777216 >>> 24 & 63 | 512
  }
  return $1 & 63;
 }
 
 function del_timer($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  label$1 : {
   label$2 : {
    label$3 : {
     if (HEAP32[$0 + 4 >> 2]) {
      $2 = $0 + 16 | 0;
      while (1) {
       $3 = HEAP32[$2 >> 2];
       if ($3 & 262144) {
        continue
       }
       $4 = HEAP32[16697];
       HEAP32[16697] = 0;
       if (HEAP32[$2 >> 2] != ($3 | 0)) {
        HEAP32[16697] = $4;
        continue;
       }
       break;
      };
      $2 = HEAP32[$0 + 4 >> 2];
      $1 = 0;
      if (!$2) {
       break label$1
      }
      $1 = HEAP32[$0 >> 2];
      if (!$1) {
       break label$3
      }
      HEAP32[$1 + 4 >> 2] = $2;
      HEAP32[$2 >> 2] = $1;
      break label$2;
     }
     return 0;
    }
    $1 = $3 >>> 22 | 0;
    if (($2 | 0) == (($1 << 2) + 8668 | 0)) {
     $3 = ($3 >>> 25 & 124) + 8596 | 0;
     $5 = HEAP32[$3 >> 2];
     (wasm2js_i32$0 = $3, wasm2js_i32$1 = __wasm_rotl_i32(-2, $1) & $5), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
    }
    HEAP32[$2 >> 2] = 0;
   }
   HEAP32[$0 >> 2] = 512;
   HEAP32[$0 + 4 >> 2] = 0;
   $1 = 1;
  }
  HEAP32[16697] = $4;
  return $1;
 }
 
 function schedule_timeout($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $1 = global$0 - 32 | 0;
  global$0 = $1;
  $2 = 2147483647;
  label$1 : {
   label$2 : {
    label$3 : {
     if (($0 | 0) == 2147483647) {
      schedule();
      break label$3;
     }
     if (($0 | 0) <= -1) {
      break label$2
     }
     HEAP32[$1 + 24 >> 2] = 0;
     HEAP32[$1 + 20 >> 2] = 11;
     HEAP32[$1 + 12 >> 2] = 0;
     HEAP32[$1 + 28 >> 2] = HEAP32[2];
     $5 = HEAP32[20745];
     $4 = $1 + 24 | 0;
     while (1) {
      $3 = HEAP32[$4 >> 2];
      if ($3 & 262144) {
       continue
      }
      $2 = HEAP32[16697];
      HEAP32[16697] = 0;
      if ($3) {
       HEAP32[16697] = $2;
       continue;
      }
      break;
     };
     $5 = $0 + $5 | 0;
     HEAP32[$1 + 16 >> 2] = $5;
     $0 = calc_wheel_index($5, HEAP32[2145]);
     $4 = ($0 << 2) + 8668 | 0;
     $3 = HEAP32[$4 >> 2];
     HEAP32[$1 + 8 >> 2] = $3;
     if ($3) {
      HEAP32[$3 + 4 >> 2] = $1 + 8
     }
     HEAP32[16697] = $2;
     $3 = $1 + 24 | 0;
     HEAP32[$3 >> 2] = $0 << 22;
     $2 = (($0 | 0) / 32 << 2) + 8596 | 0;
     HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 1 << ($0 & 31);
     HEAP32[$1 + 12 >> 2] = $4;
     HEAP32[$4 >> 2] = $1 + 8;
     schedule();
     if (HEAP32[$1 + 12 >> 2]) {
      while (1) {
       $2 = HEAP32[$3 >> 2];
       if ($2 & 262144) {
        continue
       }
       $4 = HEAP32[16697];
       HEAP32[16697] = 0;
       if (HEAP32[$3 >> 2] != ($2 | 0)) {
        HEAP32[16697] = $4;
        continue;
       }
       break;
      };
      $0 = HEAP32[$1 + 12 >> 2];
      if ($0) {
       $3 = HEAP32[$1 + 8 >> 2];
       label$12 : {
        if ($3) {
         HEAP32[$3 + 4 >> 2] = $0;
         HEAP32[$0 >> 2] = $3;
         break label$12;
        }
        $3 = $2 >>> 22 | 0;
        if ((($3 << 2) + 8668 | 0) == ($0 | 0)) {
         $2 = ($2 >>> 25 & 124) + 8596 | 0;
         $6 = HEAP32[$2 >> 2];
         (wasm2js_i32$0 = $2, wasm2js_i32$1 = __wasm_rotl_i32(-2, $3) & $6), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
        }
        HEAP32[$0 >> 2] = 0;
       }
       HEAP32[$1 + 8 >> 2] = 512;
       HEAP32[$1 + 12 >> 2] = 0;
      }
      HEAP32[16697] = $4;
     }
     $2 = $5 - HEAP32[20745] | 0;
     $0 = 0;
     if (($2 | 0) < 1) {
      break label$1
     }
    }
    $0 = $2;
    break label$1;
   }
   HEAP32[$1 >> 2] = $0;
   printk(21663, $1);
   dump_stack();
   HEAP32[HEAP32[2] >> 2] = 0;
   $0 = 0;
  }
  global$0 = $1 + 32 | 0;
  return $0;
 }
 
 function process_timeout($0) {
  $0 = $0 | 0;
  wake_up_process(HEAP32[$0 + 20 >> 2]);
 }
 
 function schedule_timeout_uninterruptible() {
  HEAP32[HEAP32[2] >> 2] = 2;
  schedule_timeout(1);
 }
 
 function run_timer_softirq($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $6 = global$0 - 48 | 0;
  global$0 = $6;
  if ((HEAP32[20745] - HEAP32[2145] | 0) >= 0) {
   HEAP32[16697] = 0;
   HEAP8[8593] = 0;
   $1 = HEAP32[2145];
   if ((HEAP32[20745] - $1 | 0) >= 0) {
    $0 = $6;
    $4 = $6;
    while (1) {
     $7 = $1 & 63 | $3 << 6;
     $8 = (($7 | 0) / 32 << 2) + 8596 | 0;
     $2 = HEAP32[$8 >> 2];
     (wasm2js_i32$0 = $8, wasm2js_i32$1 = __wasm_rotl_i32(-2, $1) & $2), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
     if ($2 & 1 << ($1 & 31)) {
      $7 = ($7 << 2) + 8668 | 0;
      $2 = HEAP32[$7 >> 2];
      HEAP32[$4 >> 2] = $2;
      if ($2) {
       HEAP32[$2 + 4 >> 2] = $4
      }
      HEAP32[$7 >> 2] = 0;
      $5 = $5 + 1 | 0;
      $4 = $4 + 4 | 0;
     }
     if (!($1 & 7)) {
      $1 = $1 >>> 3 | 0;
      $3 = $3 + 1 | 0;
      if ($3 >>> 0 < 9) {
       continue
      }
     }
     $1 = HEAP32[2145] + 1 | 0;
     HEAP32[2145] = $1;
     if ($5) {
      while (1) {
       $5 = $5 + -1 | 0;
       $4 = ($5 << 2) + $6 | 0;
       $1 = HEAP32[$4 >> 2];
       label$9 : {
        if (!$1) {
         break label$9
        }
        while (1) {
         HEAP32[2144] = $1;
         $2 = HEAP32[$1 + 4 >> 2];
         $3 = HEAP32[$1 >> 2];
         HEAP32[$2 >> 2] = $3;
         if ($3) {
          HEAP32[$3 + 4 >> 2] = $2
         }
         HEAP32[$1 >> 2] = 512;
         HEAP32[$1 + 4 >> 2] = 0;
         $3 = HEAP32[$1 + 12 >> 2];
         if (!(HEAPU8[$1 + 18 | 0] & 32)) {
          HEAP32[16697] = 1;
          $2 = HEAP32[1];
          FUNCTION_TABLE[$3]($1);
          if (HEAP32[1] != ($2 | 0)) {
           HEAP32[1] = $2
          }
          HEAP32[16697] = 0;
          $1 = HEAP32[$4 >> 2];
          if ($1) {
           continue
          }
          break label$9;
         }
         $2 = HEAP32[1];
         FUNCTION_TABLE[$3]($1);
         if (HEAP32[1] != ($2 | 0)) {
          HEAP32[1] = $2
         }
         $1 = HEAP32[$4 >> 2];
         if ($1) {
          continue
         }
         break;
        };
       }
       if ($5) {
        continue
       }
       break;
      };
      $1 = HEAP32[2145];
     }
     $5 = 0;
     $4 = $0;
     $3 = 0;
     if ((HEAP32[20745] - $1 | 0) >= 0) {
      continue
     }
     break;
    };
   }
   HEAP32[16697] = 1;
   HEAP32[2144] = 0;
  }
  global$0 = $6 + 48 | 0;
 }
 
 function ktime_get_real() {
  return ktime_get_with_offset(0) | 0;
 }
 
 function ktime_get_boottime() {
  return ktime_get_with_offset(1) | 0;
 }
 
 function ktime_get_clocktai() {
  return ktime_get_with_offset(2) | 0;
 }
 
 function __ktime_divns($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  $6 = $1 >> 31;
  $7 = $6 + $1 | 0;
  $9 = $0;
  $4 = $1 >> 31;
  $0 = $4 + $0 | 0;
  if ($0 >>> 0 < $4 >>> 0) {
   $7 = $7 + 1 | 0
  }
  $8 = $0 ^ $4;
  $0 = $6 ^ $7;
  $6 = $0;
  HEAP32[$5 + 8 >> 2] = $8;
  HEAP32[$5 + 12 >> 2] = $0;
  $0 = 0;
  if (!(($3 | 0) == 1 & $2 >>> 0 < 0 | $3 >>> 0 < 1)) {
   while (1) {
    $0 = $0 + 1 | 0;
    $4 = $3;
    $3 = $4 >> 1;
    $2 = ($4 & 1) << 31 | $2 >>> 1;
    if (!$3 & $2 >>> 0 > 4294967295 | $3 >>> 0 > 0) {
     continue
    }
    break;
   }
  }
  $4 = $5;
  $10 = $4;
  $5 = $6;
  $7 = $8;
  $6 = $0 & 31;
  if (32 <= ($0 & 63) >>> 0) {
   $3 = 0;
   $0 = $5 >>> $6 | 0;
  } else {
   $3 = $5 >>> $6 | 0;
   $0 = ((1 << $6) - 1 & $5) << 32 - $6 | $7 >>> $6;
  }
  HEAP32[$10 + 8 >> 2] = $0;
  HEAP32[$4 + 12 >> 2] = $3;
  label$3 : {
   if (!(!$3 & $0 >>> 0 > 4294967295 | $3 >>> 0 > 0)) {
    $2 = ($0 >>> 0) / ($2 >>> 0) | 0;
    HEAP32[$4 + 8 >> 2] = $2;
    HEAP32[$4 + 12 >> 2] = 0;
    $3 = 0;
    break label$3;
   }
   __div64_32($4 + 8 | 0, $2);
   $2 = HEAP32[$4 + 8 >> 2];
   $3 = HEAP32[$4 + 12 >> 2];
  }
  global$0 = $4 + 16 | 0;
  $5 = 0 - ($3 + (0 < $2 >>> 0) | 0) | 0;
  $4 = 0 - $2 | 0;
  if (($1 | 0) < 0) {
   $0 = 1
  } else {
   if (($1 | 0) <= 0) {
    $0 = $9 >>> 0 >= 0 ? 0 : 1
   } else {
    $0 = 0
   }
  }
  $1 = $0 ? $4 : $2;
  i64toi32_i32$HIGH_BITS = $0 ? $5 : $3;
  return $1;
 }
 
 function hrtimer_forward($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0;
  $14 = global$0 - 16 | 0;
  global$0 = $14;
  $9 = $1;
  $8 = HEAP32[$0 + 16 >> 2];
  $5 = $8;
  $1 = HEAP32[$0 + 20 >> 2];
  $6 = $2 - (($9 >>> 0 < $5 >>> 0) + $1 | 0) | 0;
  $5 = $9 - $5 | 0;
  $10 = $5;
  if (($6 | 0) < 0) {
   $7 = 1
  } else {
   if (($6 | 0) <= 0) {
    $7 = $10 >>> 0 >= 0 ? 0 : 1
   } else {
    $7 = 0
   }
  }
  label$1 : {
   if ($7) {
    break label$1
   }
   if (HEAP8[$0 + 40 | 0] & 1) {
    break label$1
   }
   $11 = $6;
   $10 = $4;
   $7 = $5;
   $5 = $3;
   if (($4 | 0) > 0) {
    $3 = 1
   } else {
    if (($4 | 0) >= 0) {
     $3 = $3 >>> 0 <= 4e6 ? 0 : 1
    } else {
     $3 = 0
    }
   }
   $5 = $3 ? $5 : 4e6;
   $4 = $5;
   $10 = $3 ? $10 : 0;
   $3 = $10;
   if (($11 | 0) > ($3 | 0)) {
    $3 = 1
   } else {
    $3 = ($11 | 0) >= ($3 | 0) ? ($7 >>> 0 < $4 >>> 0 ? 0 : 1) : 0
   }
   label$2 : {
    if (!$3) {
     $7 = HEAP32[$0 + 24 >> 2];
     $4 = HEAP32[$0 + 28 >> 2];
     $11 = 0;
     $3 = 1;
     break label$2;
    }
    $12 = __ktime_divns($7, $6, $5, $10);
    $1 = $0 + 16 | 0;
    $6 = $1;
    $8 = HEAP32[$1 >> 2];
    $1 = HEAP32[$1 + 4 >> 2];
    $11 = i64toi32_i32$HIGH_BITS;
    $4 = __wasm_i64_mul($12, $11, $5, $10);
    $7 = i64toi32_i32$HIGH_BITS;
    $3 = $7 + $1 | 0;
    $1 = $4 + $8 | 0;
    if ($1 >>> 0 < $4 >>> 0) {
     $3 = $3 + 1 | 0
    }
    $8 = $1;
    HEAP32[$6 >> 2] = $1;
    $1 = $3;
    HEAP32[$6 + 4 >> 2] = $3;
    $3 = $7 + HEAP32[$0 + 28 >> 2] | 0;
    $6 = $4 + HEAP32[$0 + 24 >> 2] | 0;
    if ($6 >>> 0 < $4 >>> 0) {
     $3 = $3 + 1 | 0
    }
    $7 = $6;
    HEAP32[$0 + 24 >> 2] = $6;
    $4 = $3;
    HEAP32[$0 + 28 >> 2] = $3;
    $3 = $8;
    $6 = $1;
    if (($1 | 0) > ($2 | 0)) {
     $3 = 1
    } else {
     $3 = ($6 | 0) >= ($2 | 0) ? ($3 >>> 0 <= $9 >>> 0 ? 0 : 1) : 0
    }
    if ($3) {
     break label$1
    }
    if (($6 | 0) < ($2 | 0)) {
     $2 = 1
    } else {
     $2 = ($1 | 0) <= ($2 | 0) ? ($8 >>> 0 > $9 >>> 0 ? 0 : 1) : 0
    }
    $3 = $2 + $12 | 0;
    if ($3 >>> 0 < $2 >>> 0) {
     $11 = $11 + 1 | 0
    }
   }
   $12 = $3;
   $6 = $0;
   $3 = $4 + $10 | 0;
   $9 = $5;
   $2 = $5 + $7 | 0;
   if ($2 >>> 0 < $5 >>> 0) {
    $3 = $3 + 1 | 0
   }
   $5 = $2;
   $13 = $2;
   $2 = $3;
   $15 = $3;
   if (($3 | 0) < ($4 | 0)) {
    $4 = 1
   } else {
    $4 = ($15 | 0) <= ($4 | 0) ? ($5 >>> 0 >= $7 >>> 0 ? 0 : 1) : 0
   }
   $7 = $4 ? -1 : $13;
   $3 = $4 ? 2147483647 : $3;
   if (($2 | 0) < 0) {
    $4 = 1
   } else {
    if (($2 | 0) <= 0) {
     $4 = $5 >>> 0 >= 0 ? 0 : 1
    } else {
     $4 = 0
    }
   }
   $7 = $4 ? -1 : $7;
   $4 = $4 ? 2147483647 : $3;
   $13 = $6;
   $3 = $2;
   $2 = $10;
   if (($3 | 0) < ($2 | 0)) {
    $3 = 1
   } else {
    $3 = ($3 | 0) <= ($2 | 0) ? ($5 >>> 0 >= $9 >>> 0 ? 0 : 1) : 0
   }
   HEAP32[$13 + 24 >> 2] = $3 ? -1 : $7;
   HEAP32[$6 + 28 >> 2] = $3 ? 2147483647 : $4;
   $6 = $0 + 16 | 0;
   $3 = $1 + $2 | 0;
   $0 = $8 + $9 | 0;
   if ($0 >>> 0 < $9 >>> 0) {
    $3 = $3 + 1 | 0
   }
   $4 = $0;
   $7 = $0;
   $0 = $3;
   $5 = $3;
   if (($3 | 0) < ($1 | 0)) {
    $1 = 1
   } else {
    $1 = ($5 | 0) <= ($1 | 0) ? ($4 >>> 0 >= $8 >>> 0 ? 0 : 1) : 0
   }
   $8 = $1 ? -1 : $7;
   $1 = $1 ? 2147483647 : $3;
   if (($0 | 0) < 0) {
    $3 = 1
   } else {
    if (($0 | 0) <= 0) {
     $3 = $4 >>> 0 >= 0 ? 0 : 1
    } else {
     $3 = 0
    }
   }
   $8 = $3 ? -1 : $8;
   $1 = $3 ? 2147483647 : $1;
   $3 = $6;
   if (($0 | 0) < ($2 | 0)) {
    $0 = 1
   } else {
    $0 = ($0 | 0) <= ($2 | 0) ? ($4 >>> 0 >= $9 >>> 0 ? 0 : 1) : 0
   }
   HEAP32[$3 >> 2] = $0 ? -1 : $8;
   HEAP32[$6 + 4 >> 2] = $0 ? 2147483647 : $1;
  }
  global$0 = $14 + 16 | 0;
  i64toi32_i32$HIGH_BITS = $11;
  return $12;
 }
 
 function hrtimer_start_range_ns($0, $1, $2, $3, $4, $5) {
  var $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  $12 = HEAP32[16698];
  HEAP32[16698] = 0;
  $10 = HEAP32[$0 + 36 >> 2];
  $6 = HEAPU8[$0 + 40 | 0];
  if ($6 & 1) {
   __remove_hrtimer($0, $10, $6, HEAP32[$10 >> 2] == 11008)
  }
  if ($5 & 1) {
   $8 = FUNCTION_TABLE[HEAP32[$10 + 28 >> 2]]() | 0;
   $9 = i64toi32_i32$HIGH_BITS;
   $5 = $2 + $9 | 0;
   $6 = $1 + $8 | 0;
   if ($6 >>> 0 < $1 >>> 0) {
    $5 = $5 + 1 | 0
   }
   $11 = $6;
   $7 = $1;
   $1 = $5;
   $13 = $1;
   if (($1 | 0) < ($2 | 0)) {
    $2 = 1
   } else {
    $2 = ($13 | 0) <= ($2 | 0) ? ($6 >>> 0 >= $7 >>> 0 ? 0 : 1) : 0
   }
   $7 = $2 ? -1 : $11;
   $2 = $2 ? 2147483647 : $5;
   if (($1 | 0) < 0) {
    $5 = 1
   } else {
    if (($1 | 0) <= 0) {
     $5 = $6 >>> 0 >= 0 ? 0 : 1
    } else {
     $5 = 0
    }
   }
   $7 = $5 ? -1 : $7;
   $2 = $5 ? 2147483647 : $2;
   if (($1 | 0) < ($9 | 0)) {
    $5 = 1
   } else {
    $5 = ($1 | 0) <= ($9 | 0) ? ($6 >>> 0 >= $8 >>> 0 ? 0 : 1) : 0
   }
   $1 = $5 ? -1 : $7;
   $2 = $5 ? 2147483647 : $2;
  }
  HEAP32[$0 + 24 >> 2] = $1;
  HEAP32[$0 + 28 >> 2] = $2;
  HEAP8[$0 + 40 | 0] = 1;
  $5 = HEAP32[$10 >> 2];
  HEAP32[$5 + 4 >> 2] = HEAP32[$5 + 4 >> 2] | 1 << HEAP32[$10 + 4 >> 2];
  $5 = $0;
  $6 = $2 + $4 | 0;
  $8 = $1 + $3 | 0;
  if ($8 >>> 0 < $3 >>> 0) {
   $6 = $6 + 1 | 0
  }
  $11 = $8;
  $9 = $1;
  $1 = $6;
  $7 = $1;
  if (($1 | 0) < ($2 | 0)) {
   $2 = 1
  } else {
   $2 = ($7 | 0) <= ($2 | 0) ? ($8 >>> 0 >= $9 >>> 0 ? 0 : 1) : 0
  }
  $9 = $2 ? -1 : $11;
  $2 = $2 ? 2147483647 : $6;
  if (($1 | 0) < 0) {
   $6 = 1
  } else {
   if (($1 | 0) <= 0) {
    $6 = $8 >>> 0 >= 0 ? 0 : 1
   } else {
    $6 = 0
   }
  }
  $9 = $6 ? -1 : $9;
  $2 = $6 ? 2147483647 : $2;
  $7 = $5;
  if (($1 | 0) < ($4 | 0)) {
   $1 = 1
  } else {
   $1 = ($1 | 0) <= ($4 | 0) ? ($8 >>> 0 >= $3 >>> 0 ? 0 : 1) : 0
  }
  HEAP32[$7 + 16 >> 2] = $1 ? -1 : $9;
  HEAP32[$5 + 20 >> 2] = $1 ? 2147483647 : $2;
  label$3 : {
   if (!timerqueue_add($10 + 20 | 0, $0)) {
    break label$3
   }
   $1 = $0 + 16 | 0;
   $2 = HEAP32[$1 >> 2];
   $5 = HEAP32[$1 + 4 >> 2];
   $4 = HEAP32[$0 + 36 >> 2];
   $1 = $4;
   $3 = HEAP32[$1 + 32 >> 2];
   $1 = $5 - (($2 >>> 0 < $3 >>> 0) + HEAP32[$1 + 36 >> 2] | 0) | 0;
   $2 = $2 - $3 | 0;
   $5 = $2;
   $3 = $1;
   if (($1 | 0) > 0) {
    $2 = 1
   } else {
    if (($3 | 0) >= 0) {
     $2 = $2 >>> 0 <= 0 ? 0 : 1
    } else {
     $2 = 0
    }
   }
   $1 = $2 ? $5 : 0;
   $2 = $2 ? $3 : 0;
   $5 = HEAP32[$4 >> 2];
   label$4 : {
    if (HEAPU8[$0 + 42 | 0]) {
     if (HEAPU8[$5 + 12 | 0] & 8) {
      break label$3
     }
     $3 = $1;
     $6 = HEAP32[$5 + 32 >> 2];
     $4 = $2;
     $8 = HEAP32[$5 + 36 >> 2];
     if (($2 | 0) > ($8 | 0)) {
      $7 = 1
     } else {
      $7 = ($4 | 0) >= ($8 | 0) ? ($3 >>> 0 < $6 >>> 0 ? 0 : 1) : 0
     }
     if ($7) {
      break label$3
     }
     HEAP32[$5 + 40 >> 2] = $0;
     $6 = $5 + 32 | 0;
     HEAP32[$6 >> 2] = $3;
     HEAP32[$6 + 4 >> 2] = $4;
     if (($5 | 0) != 11008) {
      break label$3
     }
     $3 = HEAP32[$5 + 16 >> 2];
     $7 = $4;
     $4 = HEAP32[$5 + 20 >> 2];
     if (($7 | 0) < ($4 | 0)) {
      $3 = 1
     } else {
      $3 = ($2 | 0) <= ($4 | 0) ? ($1 >>> 0 >= $3 >>> 0 ? 0 : 1) : 0
     }
     if ($3) {
      break label$4
     }
     break label$3;
    }
    if (($5 | 0) != 11008) {
     break label$3
    }
   }
   if (HEAPU8[11020] & 2) {
    break label$3
   }
   $3 = HEAP32[2756];
   $4 = HEAP32[2757];
   if (($2 | 0) > ($4 | 0)) {
    $3 = 1
   } else {
    $3 = ($2 | 0) >= ($4 | 0) ? ($1 >>> 0 < $3 >>> 0 ? 0 : 1) : 0
   }
   if ($3) {
    break label$3
   }
   HEAP32[2756] = $1;
   HEAP32[2757] = $2;
   HEAP32[2758] = $0;
  }
  HEAP32[16698] = $12;
 }
 
 function __remove_hrtimer($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $4 = HEAPU8[$0 + 40 | 0];
  HEAP8[$0 + 40 | 0] = $2;
  label$1 : {
   if (!($4 & 1)) {
    break label$1
   }
   $7 = HEAP32[$1 >> 2];
   if (!timerqueue_del($1 + 20 | 0, $0)) {
    (wasm2js_i32$0 = $7, wasm2js_i32$1 = HEAP32[$7 + 4 >> 2] & __wasm_rotl_i32(-2, HEAP32[$1 + 4 >> 2])), HEAP32[wasm2js_i32$0 + 4 >> 2] = wasm2js_i32$1
   }
   if (!$3) {
    break label$1
   }
   if (HEAP32[$7 + 24 >> 2] != ($0 | 0)) {
    break label$1
   }
   $10 = $7 + 24 | 0;
   $12 = $10;
   label$3 : {
    label$4 : {
     label$5 : {
      if (!(HEAPU8[$7 + 12 | 0] & 8)) {
       HEAP32[$7 + 40 >> 2] = 0;
       $1 = HEAP32[$7 + 4 >> 2];
       $0 = $1 & 240;
       if (!$0) {
        break label$5
       }
       $1 = $1 & 48 ? 4 : 6;
       $1 = ($0 >>> $1 & 1 | $1) ^ 1;
       $0 = __wasm_rotl_i32(-2, $1) & $0;
       $3 = -1;
       $2 = 2147483647;
       $8 = $7 + 24 | 0;
       while (1) {
        $4 = $7 + Math_imul($1, 40) | 0;
        $1 = HEAP32[$4 + 72 >> 2];
        label$8 : {
         if (!$1) {
          $1 = 0;
          if (!$1) {
           break label$8
          }
         }
         $6 = HEAP32[$1 + 16 >> 2];
         $5 = $4 + 80 | 0;
         $9 = HEAP32[$5 >> 2];
         $4 = $6 - $9 | 0;
         $11 = $4;
         $6 = HEAP32[$1 + 20 >> 2] - (HEAP32[$5 + 4 >> 2] + ($6 >>> 0 < $9 >>> 0) | 0) | 0;
         $5 = $6;
         if (($6 | 0) > ($2 | 0)) {
          $5 = 1
         } else {
          $5 = ($5 | 0) >= ($2 | 0) ? ($11 >>> 0 < $3 >>> 0 ? 0 : 1) : 0
         }
         if ($5) {
          break label$8
         }
         label$10 : {
          if (HEAPU8[$1 + 42 | 0]) {
           HEAP32[$7 + 40 >> 2] = $1;
           break label$10;
          }
          HEAP32[$8 >> 2] = $1;
         }
         $3 = $4;
         $2 = $6;
        }
        if ($0) {
         $9 = $0 & 65535;
         $1 = $9 ? $0 : $0 >>> 16 | 0;
         $4 = $1 & 255;
         $1 = $4 ? $1 : $1 >>> 8 | 0;
         $6 = $1 & 15;
         $1 = $6 ? $1 : $1 >>> 4 | 0;
         $5 = $1 & 3;
         $11 = (($5 ? $1 : $1 >>> 2 | 0) ^ -1) & 1;
         $1 = !$9 << 4;
         $1 = $4 ? $1 : $1 | 8;
         $1 = $6 ? $1 : $1 | 4;
         $1 = $11 + ($5 ? $1 : $1 | 2) | 0;
         $0 = __wasm_rotl_i32(-2, $1) & $0;
         continue;
        }
        break;
       };
       if (($2 | 0) > 0) {
        $0 = 1
       } else {
        if (($2 | 0) >= 0) {
         $0 = $3 >>> 0 < 1 ? 0 : 1
        } else {
         $0 = 0
        }
       }
       if ($0) {
        break label$4
       }
       $3 = 0;
       $2 = 0;
       break label$4;
      }
      $3 = -1;
      $2 = 2147483647;
      $0 = 0;
      break label$3;
     }
     $3 = -1;
     $2 = 2147483647;
    }
    $0 = HEAP32[$7 + 40 >> 2];
   }
   HEAP32[$12 >> 2] = $0;
   $1 = HEAP32[$7 + 4 >> 2];
   $4 = $1 & 15;
   if ($4) {
    $0 = $1 & 3;
    $1 = (($0 ? $1 : $1 >>> 2 | 0) & 1 | !$0 << 1) ^ 1;
    $0 = __wasm_rotl_i32(-2, $1) & $4;
    while (1) {
     $4 = $7 + Math_imul($1, 40) | 0;
     $1 = HEAP32[$4 + 72 >> 2];
     label$15 : {
      if (!$1) {
       $1 = 0;
       if (!$1) {
        break label$15
       }
      }
      $6 = HEAP32[$1 + 16 >> 2];
      $5 = $4 + 80 | 0;
      $8 = HEAP32[$5 >> 2];
      $4 = $6 - $8 | 0;
      $9 = $4;
      $6 = HEAP32[$1 + 20 >> 2] - (HEAP32[$5 + 4 >> 2] + ($6 >>> 0 < $8 >>> 0) | 0) | 0;
      $5 = $6;
      if (($6 | 0) > ($2 | 0)) {
       $5 = 1
      } else {
       $5 = ($5 | 0) >= ($2 | 0) ? ($9 >>> 0 < $3 >>> 0 ? 0 : 1) : 0
      }
      if ($5) {
       break label$15
      }
      label$17 : {
       if (HEAPU8[$1 + 42 | 0]) {
        HEAP32[$7 + 40 >> 2] = $1;
        break label$17;
       }
       HEAP32[$10 >> 2] = $1;
      }
      $3 = $4;
      $2 = $6;
     }
     if ($0) {
      $8 = $0 & 65535;
      $1 = $8 ? $0 : $0 >>> 16 | 0;
      $4 = $1 & 255;
      $1 = $4 ? $1 : $1 >>> 8 | 0;
      $6 = $1 & 15;
      $1 = $6 ? $1 : $1 >>> 4 | 0;
      $5 = $1 & 3;
      $9 = (($5 ? $1 : $1 >>> 2 | 0) ^ -1) & 1;
      $1 = !$8 << 4;
      $1 = $4 ? $1 : $1 | 8;
      $1 = $6 ? $1 : $1 | 4;
      $1 = $9 + ($5 ? $1 : $1 | 2) | 0;
      $0 = __wasm_rotl_i32(-2, $1) & $0;
      continue;
     }
     break;
    };
    $0 = HEAP32[$7 + 24 >> 2];
   }
   $4 = $3;
   if (($2 | 0) > 0) {
    $1 = 1
   } else {
    if (($2 | 0) >= 0) {
     $1 = $3 >>> 0 <= 0 ? 0 : 1
    } else {
     $1 = 0
    }
   }
   $3 = $1 ? $4 : 0;
   $2 = $1 ? $2 : 0;
   label$20 : {
    if (!$0) {
     break label$20
    }
    if (!HEAPU8[$0 + 42 | 0]) {
     break label$20
    }
    if (!(HEAPU8[$7 + 12 | 0] & 8)) {
     HEAP32[$7 + 32 >> 2] = $3;
     HEAP32[$7 + 36 >> 2] = $2;
     break label$20;
    }
    $10 = $7 + 24 | 0;
    HEAP32[$10 >> 2] = 0;
    $0 = HEAP32[$7 + 4 >> 2];
    $2 = $0 & 15;
    if ($2) {
     $1 = $0;
     $3 = $0 >>> 2 | 0;
     $0 = $0 & 3;
     $1 = (($0 ? $1 : $3) & 1 | !$0 << 1) ^ 1;
     $0 = __wasm_rotl_i32(-2, $1) & $2;
     $3 = -1;
     $2 = 2147483647;
     while (1) {
      $4 = $7 + Math_imul($1, 40) | 0;
      $1 = HEAP32[$4 + 72 >> 2];
      label$24 : {
       if (!$1) {
        $1 = 0;
        if (!$1) {
         break label$24
        }
       }
       $6 = HEAP32[$1 + 16 >> 2];
       $5 = $4 + 80 | 0;
       $8 = HEAP32[$5 >> 2];
       $4 = $6 - $8 | 0;
       $9 = $4;
       $6 = HEAP32[$1 + 20 >> 2] - (HEAP32[$5 + 4 >> 2] + ($6 >>> 0 < $8 >>> 0) | 0) | 0;
       $5 = $6;
       if (($6 | 0) > ($2 | 0)) {
        $5 = 1
       } else {
        $5 = ($5 | 0) >= ($2 | 0) ? ($9 >>> 0 < $3 >>> 0 ? 0 : 1) : 0
       }
       if ($5) {
        break label$24
       }
       label$26 : {
        if (HEAPU8[$1 + 42 | 0]) {
         HEAP32[$7 + 40 >> 2] = $1;
         break label$26;
        }
        HEAP32[$10 >> 2] = $1;
       }
       $3 = $4;
       $2 = $6;
      }
      if ($0) {
       $8 = $0 & 65535;
       $1 = $8 ? $0 : $0 >>> 16 | 0;
       $4 = $1 & 255;
       $1 = $4 ? $1 : $1 >>> 8 | 0;
       $6 = $1 & 15;
       $1 = $6 ? $1 : $1 >>> 4 | 0;
       $5 = $1 & 3;
       $9 = (($5 ? $1 : $1 >>> 2 | 0) ^ -1) & 1;
       $1 = !$8 << 4;
       $1 = $4 ? $1 : $1 | 8;
       $1 = $6 ? $1 : $1 | 4;
       $1 = $9 + ($5 ? $1 : $1 | 2) | 0;
       $0 = __wasm_rotl_i32(-2, $1) & $0;
       continue;
      }
      break;
     };
     if (($2 | 0) > 0) {
      $0 = 1
     } else {
      if (($2 | 0) >= 0) {
       $0 = $3 >>> 0 < 1 ? 0 : 1
      } else {
       $0 = 0
      }
     }
     if ($0) {
      break label$20
     }
     $3 = 0;
     $2 = 0;
     break label$20;
    }
    $3 = -1;
    $2 = 2147483647;
   }
   if (HEAP32[$7 + 16 >> 2] == ($3 | 0) & HEAP32[$7 + 20 >> 2] == ($2 | 0)) {
    break label$1
   }
   $0 = $7 + 16 | 0;
   HEAP32[$0 >> 2] = $3;
   HEAP32[$0 + 4 >> 2] = $2;
  }
 }
 
 function hrtimer_try_to_cancel($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $2 = $0 + 36 | 0;
  $3 = $0 + 40 | 0;
  label$1 : {
   while (1) {
    $1 = HEAP32[$2 >> 2];
    while (1) {
     $4 = $1 + 12 | 0;
     $5 = HEAP32[$4 >> 2];
     if ($5 & 1) {
      continue
     }
     break;
    };
    if (HEAPU8[$3 | 0]) {
     break label$1
    }
    if (HEAP32[$1 + 16 >> 2] == ($0 | 0)) {
     break label$1
    }
    if (($5 | 0) != HEAP32[$4 >> 2]) {
     continue
    }
    if (($1 | 0) != HEAP32[$2 >> 2]) {
     continue
    }
    break;
   };
   return 0;
  }
  $3 = HEAP32[16698];
  HEAP32[16698] = 0;
  $1 = $0 + 36 | 0;
  $2 = HEAP32[$1 >> 2];
  label$4 : {
   if (($0 | 0) != HEAP32[HEAP32[$1 >> 2] + 16 >> 2]) {
    $1 = 0;
    if (!(HEAP8[$0 + 40 | 0] & 1)) {
     break label$4
    }
    __remove_hrtimer($0, $2, 0, HEAP32[$2 >> 2] == 11008);
    $1 = 1;
    break label$4;
   }
   $1 = -1;
  }
  HEAP32[16698] = $3;
  return $1;
 }
 
 function hrtimer_active($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $2 = $0 + 36 | 0;
  $6 = $0 + 40 | 0;
  label$1 : {
   while (1) {
    $1 = HEAP32[$2 >> 2];
    while (1) {
     $3 = $1 + 12 | 0;
     $4 = HEAP32[$3 >> 2];
     if ($4 & 1) {
      continue
     }
     break;
    };
    $5 = 1;
    if (HEAPU8[$6 | 0]) {
     break label$1
    }
    if (HEAP32[$1 + 16 >> 2] == ($0 | 0)) {
     break label$1
    }
    if (HEAP32[$3 >> 2] != ($4 | 0)) {
     continue
    }
    if (HEAP32[$2 >> 2] != ($1 | 0)) {
     continue
    }
    break;
   };
   $5 = 0;
  }
  return $5;
 }
 
 function hrtimer_init($0) {
  var $1 = 0;
  memset($0, 0, 48);
  $1 = HEAP32[5433];
  HEAP8[$0 + 42 | 0] = 0;
  HEAP32[$0 + 36 >> 2] = Math_imul($1, 40) + 11056;
  HEAP32[$0 >> 2] = $0;
 }
 
 function __hrtimer_run_queues($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $7 = HEAP32[2753] & 240;
  if ($7) {
   while (1) {
    $8 = $7 & 65535;
    $3 = $8 ? $7 : $7 >>> 16 | 0;
    $5 = $3 & 255;
    $3 = $5 ? $3 : $3 >>> 8 | 0;
    $4 = $3 & 15;
    $3 = $4 ? $3 : $3 >>> 4 | 0;
    $9 = $3 & 3;
    $6 = (($9 ? $3 : $3 >>> 2 | 0) ^ -1) & 1;
    $3 = !$8 << 4;
    $3 = $5 ? $3 : $3 | 8;
    $3 = $4 ? $3 : $3 | 4;
    $3 = $6 + ($9 ? $3 : $3 | 2) | 0;
    $7 = __wasm_rotl_i32(-2, $3) & $7;
    $4 = Math_imul($3, 40);
    $10 = $4 + 11080 | 0;
    $3 = HEAP32[$10 >> 2];
    label$3 : {
     if (!$3) {
      break label$3
     }
     $9 = $4 + 11076 | 0;
     $5 = $4 + 11088 | 0;
     $8 = $0 + HEAP32[$5 >> 2] | 0;
     $5 = HEAP32[$5 + 4 >> 2] + $1 | 0;
     $11 = $8 >>> 0 < $0 >>> 0 ? $5 + 1 | 0 : $5;
     $12 = $4 + 11060 | 0;
     $13 = $4 + 11056 | 0;
     $5 = $4 + 11068 | 0;
     $14 = $4 + 11072 | 0;
     while (1) {
      $4 = HEAP32[$3 + 24 >> 2];
      $6 = HEAP32[$3 + 28 >> 2];
      if (($11 | 0) < ($6 | 0)) {
       $4 = 1
      } else {
       $4 = ($11 | 0) <= ($6 | 0) ? ($8 >>> 0 >= $4 >>> 0 ? 0 : 1) : 0
      }
      if ($4) {
       break label$3
      }
      HEAP32[$14 >> 2] = $3;
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
      $4 = $3 + 40 | 0;
      $6 = HEAPU8[$4 | 0];
      HEAP8[$4 | 0] = 0;
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
      label$5 : {
       if (!($6 & 1)) {
        break label$5
       }
       $6 = HEAP32[$13 >> 2];
       if (timerqueue_del($9, $3)) {
        break label$5
       }
       (wasm2js_i32$0 = $6, wasm2js_i32$1 = HEAP32[$6 + 4 >> 2] & __wasm_rotl_i32(-2, HEAP32[$12 >> 2])), HEAP32[wasm2js_i32$0 + 4 >> 2] = wasm2js_i32$1;
      }
      HEAP32[16698] = $2;
      $6 = FUNCTION_TABLE[HEAP32[$3 + 32 >> 2]]($3) | 0;
      HEAP32[16698] = 0;
      label$6 : {
       if (!$6) {
        break label$6
       }
       if (HEAP8[$4 | 0] & 1) {
        break label$6
       }
       HEAP8[$4 | 0] = 1;
       $4 = HEAP32[$13 >> 2];
       HEAP32[$4 + 4 >> 2] = HEAP32[$4 + 4 >> 2] | 1 << HEAP32[$12 >> 2];
       timerqueue_add($9, $3);
      }
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
      HEAP32[$14 >> 2] = 0;
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
      $3 = HEAP32[$10 >> 2];
      if ($3) {
       continue
      }
      break;
     };
    }
    if ($7) {
     continue
    }
    break;
   }
  }
 }
 
 function hrtimers_prepare_cpu() {
  HEAP32[2839] = 0;
  HEAP32[2840] = 0;
  HEAP32[2834] = 11008;
  HEAP32[2829] = 0;
  HEAP32[2830] = 0;
  HEAP32[2824] = 11008;
  HEAP32[2819] = 0;
  HEAP32[2820] = 0;
  HEAP32[2814] = 11008;
  HEAP32[2809] = 0;
  HEAP32[2810] = 0;
  HEAP32[2804] = 11008;
  HEAP32[2799] = 0;
  HEAP32[2800] = 0;
  HEAP32[2794] = 11008;
  HEAP32[2789] = 0;
  HEAP32[2790] = 0;
  HEAP32[2784] = 11008;
  HEAP32[2779] = 0;
  HEAP32[2780] = 0;
  HEAP32[2774] = 11008;
  HEAP32[2769] = 0;
  HEAP32[2770] = 0;
  HEAP32[2764] = 11008;
  HEAP32[2752] = 0;
  HEAP32[2753] = 0;
  HEAP32[2760] = -1;
  HEAP32[2761] = 2147483647;
  HEAP32[2756] = -1;
  HEAP32[2757] = 2147483647;
  HEAP8[11020] = HEAPU8[11020] & 250;
  HEAP32[2762] = 0;
  HEAP32[2758] = 0;
 }
 
 function hrtimer_run_softirq($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $9 = HEAP32[16698];
  HEAP32[16698] = 0;
  $0 = ktime_get_update_offsets_now();
  $3 = HEAP32[2783];
  HEAP32[2822] = HEAP32[2782];
  HEAP32[2823] = $3;
  $3 = HEAP32[2793];
  HEAP32[2832] = HEAP32[2792];
  HEAP32[2833] = $3;
  $3 = HEAP32[2803];
  HEAP32[2842] = HEAP32[2802];
  HEAP32[2843] = $3;
  __hrtimer_run_queues($0, i64toi32_i32$HIGH_BITS, $9);
  HEAP8[11020] = HEAPU8[11020] & 247;
  HEAP32[2762] = 0;
  $0 = HEAP32[2753];
  $5 = $0 & 240;
  label$1 : {
   if (!$5) {
    break label$1
   }
   $0 = $0 & 48 ? 4 : 6;
   $2 = ($5 >>> $0 & 1 | $0) ^ 1;
   $0 = -1;
   $3 = 2147483647;
   while (1) {
    $5 = __wasm_rotl_i32(-2, $2) & $5;
    $1 = Math_imul($2, 40);
    $2 = HEAP32[$1 + 11080 >> 2];
    label$3 : {
     if (!$2) {
      $2 = 0;
      if (!$2) {
       break label$3
      }
     }
     $4 = HEAP32[$2 + 16 >> 2];
     $6 = $1 + 11088 | 0;
     $7 = HEAP32[$6 >> 2];
     $1 = $4 - $7 | 0;
     $8 = $1;
     $4 = HEAP32[$2 + 20 >> 2] - (HEAP32[$6 + 4 >> 2] + ($4 >>> 0 < $7 >>> 0) | 0) | 0;
     $6 = $4;
     if (($4 | 0) > ($3 | 0)) {
      $6 = 1
     } else {
      $6 = ($6 | 0) >= ($3 | 0) ? ($8 >>> 0 < $0 >>> 0 ? 0 : 1) : 0
     }
     if ($6) {
      break label$3
     }
     label$5 : {
      if (HEAPU8[$2 + 42 | 0]) {
       HEAP32[2762] = $2;
       break label$5;
      }
      HEAP32[2758] = $2;
     }
     $0 = $1;
     $3 = $4;
    }
    if ($5) {
     $7 = $5 & 65535;
     $1 = $7 ? $5 : $5 >>> 16 | 0;
     $4 = $1 & 255;
     $1 = $4 ? $1 : $1 >>> 8 | 0;
     $2 = $1 & 15;
     $1 = $2 ? $1 : $1 >>> 4 | 0;
     $6 = $1 & 3;
     $8 = (($6 ? $1 : $1 >>> 2 | 0) ^ -1) & 1;
     $1 = !$7 << 4;
     $1 = $4 ? $1 : $1 | 8;
     $1 = $2 ? $1 : $1 | 4;
     $2 = $8 + ($6 ? $1 : $1 | 2) | 0;
     continue;
    }
    break;
   };
   if (($0 | 0) == -1 & ($3 | 0) == 2147483647) {
    break label$1
   }
   $4 = HEAP32[2762];
   $3 = $4;
   $1 = HEAP32[$3 + 16 >> 2];
   $2 = HEAP32[$3 + 36 >> 2];
   $6 = HEAP32[$2 + 32 >> 2];
   $0 = $1 - $6 | 0;
   $3 = HEAP32[$3 + 20 >> 2] - (HEAP32[$2 + 36 >> 2] + ($1 >>> 0 < $6 >>> 0) | 0) | 0;
   $1 = $3;
   $5 = $0;
   if (($1 | 0) > 0) {
    $0 = 1
   } else {
    if (($3 | 0) >= 0) {
     $0 = $0 >>> 0 <= 0 ? 0 : 1
    } else {
     $0 = 0
    }
   }
   $3 = $0 ? $5 : 0;
   $0 = $0 ? $1 : 0;
   $1 = HEAP32[$2 >> 2];
   label$8 : {
    if (HEAPU8[$4 + 42 | 0]) {
     if (HEAPU8[$1 + 12 | 0] & 8) {
      break label$1
     }
     $2 = HEAP32[$1 + 32 >> 2];
     $5 = HEAP32[$1 + 36 >> 2];
     if (($0 | 0) > ($5 | 0)) {
      $2 = 1
     } else {
      $2 = ($0 | 0) >= ($5 | 0) ? ($3 >>> 0 < $2 >>> 0 ? 0 : 1) : 0
     }
     if ($2) {
      break label$1
     }
     HEAP32[$1 + 40 >> 2] = $4;
     $2 = $1 + 32 | 0;
     HEAP32[$2 >> 2] = $3;
     HEAP32[$2 + 4 >> 2] = $0;
     if (($1 | 0) != 11008) {
      break label$1
     }
     $2 = HEAP32[$1 + 16 >> 2];
     $1 = HEAP32[$1 + 20 >> 2];
     if (($0 | 0) < ($1 | 0)) {
      $1 = 1
     } else {
      $1 = ($0 | 0) <= ($1 | 0) ? ($3 >>> 0 >= $2 >>> 0 ? 0 : 1) : 0
     }
     if ($1) {
      break label$8
     }
     break label$1;
    }
    if (($1 | 0) != 11008) {
     break label$1
    }
   }
   if (HEAPU8[11020] & 2) {
    break label$1
   }
   $1 = HEAP32[2756];
   $2 = HEAP32[2757];
   if (($0 | 0) > ($2 | 0)) {
    $1 = 1
   } else {
    $1 = ($0 | 0) >= ($2 | 0) ? ($3 >>> 0 < $1 >>> 0 ? 0 : 1) : 0
   }
   if ($1) {
    break label$1
   }
   HEAP32[2756] = $3;
   HEAP32[2757] = $0;
   HEAP32[2758] = $4;
  }
  HEAP32[16698] = $9;
 }
 
 function ktime_get() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  while (1) {
   $4 = HEAP32[16704];
   if ($4 & 1) {
    continue
   }
   $5 = HEAP32[16716];
   $6 = HEAP32[16717];
   $0 = HEAP32[16706];
   $0 = FUNCTION_TABLE[HEAP32[$0 >> 2]]($0) | 0;
   $7 = i64toi32_i32$HIGH_BITS;
   $2 = HEAP32[16713];
   $1 = HEAP32[16714];
   $8 = HEAP32[16715];
   $9 = HEAP32[16712];
   $10 = HEAP32[16708];
   $11 = HEAP32[16709];
   $3 = HEAP32[16710];
   $12 = HEAP32[16711];
   if (HEAP32[16704] != ($4 | 0)) {
    continue
   }
   break;
  };
  $3 = __wasm_i64_mul($10 & $0 - $3, $11 & $7 - ($12 + ($0 >>> 0 < $3 >>> 0) | 0), $9, 0) + $1 | 0;
  $0 = $8 + i64toi32_i32$HIGH_BITS | 0;
  $0 = $3 >>> 0 < $1 >>> 0 ? $0 + 1 | 0 : $0;
  $1 = $2 & 31;
  if (32 <= ($2 & 63) >>> 0) {
   $2 = 0;
   $1 = $0 >>> $1 | 0;
  } else {
   $2 = $0 >>> $1 | 0;
   $1 = ((1 << $1) - 1 & $0) << 32 - $1 | $3 >>> $1;
  }
  $1 = $1 + $5 | 0;
  $0 = $2 + $6 | 0;
  $0 = $1 >>> 0 < $5 >>> 0 ? $0 + 1 | 0 : $0;
  i64toi32_i32$HIGH_BITS = $0;
  return $1 | 0;
 }
 
 function ktime_get_with_offset($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0;
  $0 = HEAP32[($0 << 2) + 21792 >> 2];
  while (1) {
   $3 = HEAP32[16704];
   if ($3 & 1) {
    continue
   }
   $7 = HEAP32[$0 >> 2];
   $8 = HEAP32[$0 + 4 >> 2];
   $5 = HEAP32[16716];
   $9 = HEAP32[16717];
   $1 = HEAP32[16706];
   $1 = FUNCTION_TABLE[HEAP32[$1 >> 2]]($1) | 0;
   $10 = i64toi32_i32$HIGH_BITS;
   $4 = HEAP32[16713];
   $2 = HEAP32[16714];
   $11 = HEAP32[16715];
   $12 = HEAP32[16712];
   $13 = HEAP32[16708];
   $14 = HEAP32[16709];
   $6 = HEAP32[16710];
   $15 = HEAP32[16711];
   if (($3 | 0) != HEAP32[16704]) {
    continue
   }
   break;
  };
  $0 = $8 + $9 | 0;
  $3 = $5 + $7 | 0;
  if ($3 >>> 0 < $5 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $5 = $0;
  $1 = __wasm_i64_mul($13 & $1 - $6, $14 & $10 - ($15 + ($1 >>> 0 < $6 >>> 0) | 0), $12, 0) + $2 | 0;
  $0 = $11 + i64toi32_i32$HIGH_BITS | 0;
  $0 = $1 >>> 0 < $2 >>> 0 ? $0 + 1 | 0 : $0;
  $2 = $1;
  $1 = $4 & 31;
  if (32 <= ($4 & 63) >>> 0) {
   $4 = 0;
   $2 = $0 >>> $1 | 0;
  } else {
   $4 = $0 >>> $1 | 0;
   $2 = ((1 << $1) - 1 & $0) << 32 - $1 | $2 >>> $1;
  }
  $1 = $2 + $3 | 0;
  $0 = $5 + $4 | 0;
  i64toi32_i32$HIGH_BITS = $1 >>> 0 < $2 >>> 0 ? $0 + 1 | 0 : $0;
  return $1;
 }
 
 function timekeeping_update() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $2 = ntp_get_next_leap();
  $0 = i64toi32_i32$HIGH_BITS;
  $1 = $0;
  HEAP32[16752] = $2;
  HEAP32[16753] = $0;
  if (!(($2 | 0) == -1 & ($0 | 0) == 2147483647)) {
   $0 = HEAP32[16742];
   $1 = $1 - (HEAP32[16743] + ($2 >>> 0 < $0 >>> 0) | 0) | 0;
   HEAP32[16752] = $2 - $0;
   HEAP32[16753] = $1;
  }
  (wasm2js_i32$0 = 66920, wasm2js_i32$1 = __wasm_i64_mul(HEAP32[16754], HEAP32[16755], 1e9, 0)), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  HEAP32[16731] = i64toi32_i32$HIGH_BITS;
  $0 = HEAP32[16739] + HEAP32[16735] | 0;
  $1 = HEAP32[16734];
  $2 = $1 + HEAP32[16738] | 0;
  if ($2 >>> 0 < $1 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $1 = __wasm_i64_mul($2, $0, 1e9, 0);
  $0 = i64toi32_i32$HIGH_BITS;
  $4 = HEAP32[16740];
  $3 = $4;
  $1 = $1 + $3 | 0;
  if ($1 >>> 0 < $3 >>> 0) {
   $0 = $0 + 1 | 0
  }
  HEAP32[16716] = $1;
  HEAP32[16717] = $0;
  $1 = HEAP32[16715];
  $3 = HEAP32[16714];
  $5 = HEAP32[16713];
  $0 = $5 & 31;
  if (32 <= ($5 & 63) >>> 0) {
   $0 = $1 >>> $0 | 0
  } else {
   $0 = ((1 << $0) - 1 & $1) << 32 - $0 | $3 >>> $0
  }
  HEAP32[16736] = $2 + ($0 + $4 >>> 0 > 999999999);
  atomic_notifier_call_chain(67088, 1, 66824);
  $2 = HEAP32[16743] + HEAP32[16717] | 0;
  $0 = HEAP32[16716];
  $1 = $0 + HEAP32[16742] | 0;
  if ($1 >>> 0 < $0 >>> 0) {
   $2 = $2 + 1 | 0
  }
  HEAP32[16718] = $1;
  HEAP32[16719] = $2;
  HEAP32[2848] = HEAP32[2848] + 1;
  memcpy(11400, 66824, 56);
  HEAP32[2848] = HEAP32[2848] + 1;
  memcpy(11456, 11400, 56);
  HEAP32[2880] = HEAP32[2880] + 1;
  memcpy(11528, 66880, 56);
  HEAP32[2880] = HEAP32[2880] + 1;
  memcpy(11584, 11528, 56);
  HEAP32[16749] = HEAP32[16749] + 1;
  memcpy(67104, 66824, 264);
 }
 
 function tk_set_wall_to_mono($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  $1 = HEAP32[16738];
  $2 = 0 - $1 | 0;
  $3 = 0 - (HEAP32[16739] + (0 < $1 >>> 0) | 0) | 0;
  $1 = 0 - HEAP32[16740] | 0;
  set_normalized_timespec64($4, $2, $3, $1, $1 >> 31);
  $2 = $0 + 8 | 0;
  $1 = $2;
  $3 = HEAP32[$1 + 4 >> 2];
  HEAP32[16740] = HEAP32[$1 >> 2];
  HEAP32[16741] = $3;
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = HEAP32[$0 >> 2];
  HEAP32[16738] = $0;
  HEAP32[16739] = $1;
  $3 = 0 - $0 | 0;
  $1 = 0 - ((0 < $0 >>> 0) + $1 | 0) | 0;
  $0 = 0 - HEAP32[$2 >> 2] | 0;
  set_normalized_timespec64($4, $3, $1, $0, $0 >> 31);
  $0 = HEAP32[$4 + 4 >> 2];
  $1 = $0;
  $2 = HEAP32[$4 >> 2];
  $3 = __wasm_i64_mul($2, $0, 1e9, 0);
  $0 = i64toi32_i32$HIGH_BITS;
  $5 = HEAP32[$4 + 8 >> 2];
  $3 = $3 + $5 | 0;
  if ($3 >>> 0 < $5 >>> 0) {
   $0 = $0 + 1 | 0
  }
  if (($1 | 0) > 2) {
   $2 = 1
  } else {
   if (($1 | 0) >= 2) {
    $2 = $2 >>> 0 <= 633437443 ? 0 : 1
   } else {
    $2 = 0
   }
  }
  $1 = $2 ? -1 : $3;
  $0 = $2 ? 2147483647 : $0;
  $2 = $0;
  HEAP32[16742] = $1;
  HEAP32[16743] = $0;
  $0 = HEAP32[16748];
  $0 = __wasm_i64_mul($0, $0 >> 31, 1e9, 0) + $1 | 0;
  HEAP32[16746] = $0;
  $2 = $2 + i64toi32_i32$HIGH_BITS | 0;
  HEAP32[16747] = $0 >>> 0 < $1 >>> 0 ? $2 + 1 | 0 : $2;
  global$0 = $4 + 16 | 0;
 }
 
 function tk_setup_internals($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  $4 = HEAP32[16706];
  HEAP32[16706] = $0;
  $5 = HEAP32[$0 + 12 >> 2];
  HEAP32[16708] = HEAP32[$0 + 8 >> 2];
  HEAP32[16709] = $5;
  HEAP8[67e3] = HEAPU8[67e3] + 1;
  $5 = HEAP32[16706];
  $1 = FUNCTION_TABLE[HEAP32[$5 >> 2]]($5) | 0;
  HEAP32[16720] = $0;
  HEAP32[16710] = $1;
  $5 = i64toi32_i32$HIGH_BITS;
  HEAP32[16711] = $5;
  HEAP32[16724] = $1;
  HEAP32[16725] = $5;
  $5 = HEAP32[$0 + 12 >> 2];
  HEAP32[16722] = HEAP32[$0 + 8 >> 2];
  HEAP32[16723] = $5;
  $5 = HEAP32[$0 + 20 >> 2];
  $1 = $5 & 31;
  if (32 <= ($5 & 63) >>> 0) {
   $5 = 4e6 << $1;
   $7 = 0;
  } else {
   $5 = (1 << $1) - 1 & 4e6 >>> 32 - $1;
   $7 = 4e6 << $1;
  }
  $8 = $5;
  $3 = HEAP32[$0 + 16 >> 2];
  $2 = $3 >>> 1 | 0;
  $1 = $2 + $7 | 0;
  if ($1 >>> 0 < $2 >>> 0) {
   $8 = $8 + 1 | 0
  }
  $2 = $1;
  HEAP32[$6 + 8 >> 2] = $1;
  HEAP32[$6 + 12 >> 2] = $8;
  label$1 : {
   if (!(!$8 & $1 >>> 0 > 4294967295 | $8 >>> 0 > 0)) {
    $2 = ($2 >>> 0) / ($3 >>> 0) | 0;
    HEAP32[$6 + 8 >> 2] = $2;
    HEAP32[$6 + 12 >> 2] = 0;
    $1 = 0;
    break label$1;
   }
   __div64_32($6 + 8 | 0, $3);
   $2 = HEAP32[$6 + 8 >> 2];
   $1 = HEAP32[$6 + 12 >> 2];
  }
  if (!(($2 | 0) != 0 | ($1 | 0) != 0)) {
   $2 = 1;
   HEAP32[$6 + 8 >> 2] = 1;
   HEAP32[$6 + 12 >> 2] = 0;
   $1 = 0;
  }
  HEAP32[16756] = $2;
  HEAP32[16757] = $1;
  $8 = HEAP32[$0 + 16 >> 2];
  $1 = __wasm_i64_mul($2, $1, $8, 0);
  $3 = i64toi32_i32$HIGH_BITS;
  HEAP32[16762] = $1;
  HEAP32[16763] = $3;
  HEAP32[16758] = $1;
  HEAP32[16759] = $3;
  HEAP32[16760] = $7 - $1;
  HEAP32[16761] = $5 - ($3 + ($7 >>> 0 < $1 >>> 0) | 0);
  $9 = HEAP32[$0 + 20 >> 2];
  if ($4) {
   $0 = $9 - HEAP32[$4 + 20 >> 2] | 0;
   label$5 : {
    if (($0 | 0) > -1) {
     $2 = HEAP32[16715];
     $3 = HEAP32[16714];
     $1 = $0;
     $4 = $1 & 31;
     if (32 <= ($1 & 63) >>> 0) {
      $1 = $3 << $4;
      $2 = 0;
     } else {
      $1 = (1 << $4) - 1 & $3 >>> 32 - $4 | $2 << $4;
      $2 = $3 << $4;
     }
     HEAP32[16714] = $2;
     HEAP32[16715] = $1;
     $1 = HEAP32[16729];
     $2 = HEAP32[16728];
     $3 = $0 & 31;
     if (32 <= ($0 & 63) >>> 0) {
      $1 = $2 << $3;
      $0 = 0;
     } else {
      $1 = (1 << $3) - 1 & $2 >>> 32 - $3 | $1 << $3;
      $0 = $2 << $3;
     }
     break label$5;
    }
    $3 = HEAP32[16715];
    $2 = HEAP32[16714];
    $0 = 0 - $0 | 0;
    $1 = $0;
    $4 = $1 & 31;
    if (32 <= ($1 & 63) >>> 0) {
     $1 = 0;
     $2 = $3 >>> $4 | 0;
    } else {
     $1 = $3 >>> $4 | 0;
     $2 = ((1 << $4) - 1 & $3) << 32 - $4 | $2 >>> $4;
    }
    HEAP32[16714] = $2;
    HEAP32[16715] = $1;
    $3 = HEAP32[16729];
    $2 = HEAP32[16728];
    $4 = $0 & 31;
    if (32 <= ($0 & 63) >>> 0) {
     $1 = 0;
     $0 = $3 >>> $4 | 0;
    } else {
     $1 = $3 >>> $4 | 0;
     $0 = ((1 << $4) - 1 & $3) << 32 - $4 | $2 >>> $4;
    }
   }
   HEAP32[16728] = $0;
   HEAP32[16729] = $1;
  }
  HEAP32[16766] = 0;
  HEAP32[16767] = 0;
  HEAP32[16727] = $9;
  HEAP32[16713] = $9;
  HEAP32[16769] = 0;
  HEAP32[16770] = 0;
  HEAP32[16726] = $8;
  HEAP32[16712] = $8;
  $0 = 32 - $9 | 0;
  HEAP32[16768] = $0;
  $1 = $7;
  $7 = $0 & 31;
  if (32 <= ($0 & 63) >>> 0) {
   $0 = $1 << $7;
   $1 = 0;
  } else {
   $0 = (1 << $7) - 1 & $1 >>> 32 - $7 | $5 << $7;
   $1 = $1 << $7;
  }
  HEAP32[16764] = $1;
  HEAP32[16765] = $0;
  global$0 = $6 + 16 | 0;
 }
 
 function read_persistent_clock64($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $1 = global$0 - 32 | 0;
  global$0 = $1;
  $2 = $1 + 16 | 0;
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  $3 = $1 + 32 | 0;
  $4 = HEAP32[$3 + 4 >> 2];
  $2 = $1 + 8 | 0;
  HEAP32[$2 >> 2] = HEAP32[$3 >> 2];
  HEAP32[$2 + 4 >> 2] = $4;
  $3 = HEAP32[$1 + 20 >> 2];
  $4 = HEAP32[$1 + 16 >> 2];
  HEAP32[$1 + 24 >> 2] = $4;
  HEAP32[$1 + 28 >> 2] = $3;
  HEAP32[$1 >> 2] = $4;
  HEAP32[$1 + 4 >> 2] = $3;
  $3 = HEAP32[$2 + 4 >> 2];
  $4 = $0 + 8 | 0;
  HEAP32[$4 >> 2] = HEAP32[$2 >> 2];
  HEAP32[$4 + 4 >> 2] = $3;
  $2 = HEAP32[$1 + 4 >> 2];
  HEAP32[$0 >> 2] = HEAP32[$1 >> 2];
  HEAP32[$0 + 4 >> 2] = $2;
  global$0 = $1 + 32 | 0;
 }
 
 function read_persistent_wall_and_boot_offset($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  read_persistent_clock64($0);
  ns_to_timespec64($2, sched_clock(), i64toi32_i32$HIGH_BITS);
  $3 = $2 + 8 | 0;
  $4 = HEAP32[$3 + 4 >> 2];
  $0 = $1 + 8 | 0;
  HEAP32[$0 >> 2] = HEAP32[$3 >> 2];
  HEAP32[$0 + 4 >> 2] = $4;
  $0 = HEAP32[$2 + 4 >> 2];
  HEAP32[$1 >> 2] = HEAP32[$2 >> 2];
  HEAP32[$1 + 4 >> 2] = $0;
  global$0 = $2 + 16 | 0;
 }
 
 function timekeeping_init() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $0 = global$0 - 80 | 0;
  global$0 = $0;
  read_persistent_wall_and_boot_offset($0 - -64 | 0, $0 + 48 | 0);
  $2 = HEAP32[$0 + 72 >> 2];
  $4 = HEAP32[$0 + 64 >> 2];
  $3 = $4;
  label$1 : {
   label$2 : {
    $5 = HEAP32[$0 + 68 >> 2];
    $1 = $5;
    if (($1 | 0) < 0) {
     $3 = 1
    } else {
     if (($5 | 0) <= 0) {
      $3 = $3 >>> 0 >= 0 ? 0 : 1
     } else {
      $3 = 0
     }
    }
    if ($3) {
     break label$2
    }
    if (($1 | 0) == 2 & $4 >>> 0 > 633437443 | $1 >>> 0 > 2) {
     break label$2
    }
    if ($2 >>> 0 > 999999999) {
     break label$2
    }
    $3 = __wasm_i64_mul($4, $1, 1e9, 0) + $2 | 0;
    $5 = i64toi32_i32$HIGH_BITS + ($2 >> 31) | 0;
    $5 = $3 >>> 0 < $2 >>> 0 ? $5 + 1 | 0 : $5;
    if (($5 | 0) < 0) {
     $3 = 1
    } else {
     if (($5 | 0) <= 0) {
      $3 = $3 >>> 0 >= 1 ? 0 : 1
     } else {
      $3 = 0
     }
    }
    if ($3) {
     break label$2
    }
    HEAP8[67092] = 1;
    break label$1;
   }
   if ((__wasm_i64_mul($4, $1, 1e9, 0) | 0) == (0 - $2 | 0)) {
    $3 = (i64toi32_i32$HIGH_BITS | 0) == (0 - (($2 >> 31) + (0 < $2 >>> 0) | 0) | 0)
   } else {
    $3 = 0
   }
   if ($3) {
    break label$1
   }
   $2 = 0;
   printk(21804, 0);
   HEAP32[$0 + 72 >> 2] = 0;
   $4 = 0;
   $1 = 0;
   HEAP32[$0 + 64 >> 2] = 0;
   HEAP32[$0 + 68 >> 2] = 0;
  }
  $3 = HEAP32[$0 + 48 >> 2];
  $7 = $3;
  $5 = HEAP32[$0 + 52 >> 2];
  $6 = $5;
  if (($1 | 0) < ($5 | 0)) {
   $6 = 1
  } else {
   $6 = ($1 | 0) <= ($6 | 0) ? ($4 >>> 0 >= $7 >>> 0 ? 0 : 1) : 0
  }
  label$3 : {
   if (!$6) {
    $7 = HEAP32[$0 + 56 >> 2];
    if (($1 | 0) > ($5 | 0)) {
     $6 = 1
    } else {
     $6 = ($1 | 0) >= ($5 | 0) ? ($4 >>> 0 <= $3 >>> 0 ? 0 : 1) : 0
    }
    if ($6) {
     break label$3
    }
    if (($2 | 0) >= ($7 | 0)) {
     break label$3
    }
   }
   $7 = 0;
   HEAP32[$0 + 56 >> 2] = 0;
   $3 = 0;
   $5 = 0;
   HEAP32[$0 + 48 >> 2] = 0;
   HEAP32[$0 + 52 >> 2] = 0;
  }
  $5 = $5 - (($3 >>> 0 < $4 >>> 0) + $1 | 0) | 0;
  $1 = $7 - $2 | 0;
  set_normalized_timespec64($0 + 16 | 0, $3 - $4 | 0, $5, $1, $1 >> 31);
  $2 = $0 + 24 | 0;
  $4 = HEAP32[$2 + 4 >> 2];
  $1 = $0 + 40 | 0;
  HEAP32[$1 >> 2] = HEAP32[$2 >> 2];
  HEAP32[$1 + 4 >> 2] = $4;
  $2 = HEAP32[$0 + 20 >> 2];
  HEAP32[$0 + 32 >> 2] = HEAP32[$0 + 16 >> 2];
  HEAP32[$0 + 36 >> 2] = $2;
  HEAP32[16704] = HEAP32[16704] + 1;
  ntp_clear();
  $2 = HEAP32[2956];
  if ($2) {
   FUNCTION_TABLE[$2](11760) | 0
  }
  tk_setup_internals(11760);
  HEAP32[16754] = 0;
  HEAP32[16755] = 0;
  $2 = HEAP32[$0 + 68 >> 2];
  HEAP32[16734] = HEAP32[$0 + 64 >> 2];
  HEAP32[16735] = $2;
  $2 = HEAP32[$1 + 4 >> 2];
  $4 = $0 + 8 | 0;
  HEAP32[$4 >> 2] = HEAP32[$1 >> 2];
  HEAP32[$4 + 4 >> 2] = $2;
  $1 = HEAP32[$0 + 72 >> 2];
  $4 = $1;
  $1 = $1 >> 31;
  $3 = HEAP32[16713];
  $2 = $3 & 31;
  if (32 <= ($3 & 63) >>> 0) {
   $1 = $4 << $2;
   $3 = 0;
  } else {
   $1 = (1 << $2) - 1 & $4 >>> 32 - $2 | $1 << $2;
   $3 = $4 << $2;
  }
  HEAP32[16714] = $3;
  HEAP32[16715] = $1;
  $1 = HEAP32[$0 + 36 >> 2];
  HEAP32[$0 >> 2] = HEAP32[$0 + 32 >> 2];
  HEAP32[$0 + 4 >> 2] = $1;
  tk_set_wall_to_mono($0);
  timekeeping_update();
  HEAP32[16704] = HEAP32[16704] + 1;
  global$0 = $0 + 80 | 0;
 }
 
 function ktime_get_update_offsets_now() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  while (1) {
   $5 = HEAP32[16704];
   if ($5 & 1) {
    continue
   }
   $3 = HEAP32[16716];
   $6 = HEAP32[16717];
   $0 = HEAP32[16706];
   $1 = FUNCTION_TABLE[HEAP32[$0 >> 2]]($0) | 0;
   $0 = HEAP32[16710];
   $2 = __wasm_i64_mul($1 - $0 & HEAP32[16708], HEAP32[16709] & i64toi32_i32$HIGH_BITS - (HEAP32[16711] + ($1 >>> 0 < $0 >>> 0) | 0), HEAP32[16712], 0);
   $1 = i64toi32_i32$HIGH_BITS + HEAP32[16715] | 0;
   $0 = HEAP32[16714];
   $2 = $2 + $0 | 0;
   if ($2 >>> 0 < $0 >>> 0) {
    $0 = $1 + 1 | 0
   } else {
    $0 = $1
   }
   $1 = HEAP32[16713];
   $4 = $1 & 31;
   if (32 <= ($1 & 63) >>> 0) {
    $1 = 0;
    $2 = $0 >>> $4 | 0;
   } else {
    $1 = $0 >>> $4 | 0;
    $2 = ((1 << $4) - 1 & $0) << 32 - $4 | $2 >>> $4;
   }
   $3 = $3 + $2 | 0;
   $0 = $1 + $6 | 0;
   $2 = $3 >>> 0 < $2 >>> 0 ? $0 + 1 | 0 : $0;
   $1 = HEAP32[16749];
   if (($1 | 0) != HEAP32[2754]) {
    $0 = HEAP32[16743];
    HEAP32[2782] = HEAP32[16742];
    HEAP32[2783] = $0;
    $0 = HEAP32[16745];
    HEAP32[2792] = HEAP32[16744];
    HEAP32[2793] = $0;
    HEAP32[2754] = $1;
    $0 = HEAP32[16747];
    HEAP32[2802] = HEAP32[16746];
    HEAP32[2803] = $0;
   }
   $1 = HEAP32[16752];
   $0 = HEAP32[16753];
   if (($2 | 0) < ($0 | 0)) {
    $0 = 1
   } else {
    $0 = ($2 | 0) <= ($0 | 0) ? ($3 >>> 0 >= $1 >>> 0 ? 0 : 1) : 0
   }
   if (!$0) {
    $1 = HEAP32[16743] + -1 | 0;
    $0 = HEAP32[16742] + -1e9 | 0;
    if ($0 >>> 0 < 3294967296) {
     $1 = $1 + 1 | 0
    }
    HEAP32[2782] = $0;
    HEAP32[2783] = $1;
   }
   if (HEAP32[16704] != ($5 | 0)) {
    continue
   }
   break;
  };
  i64toi32_i32$HIGH_BITS = $2;
  return $3;
 }
 
 function dummy_clock_read($0) {
  $0 = $0 | 0;
  i64toi32_i32$HIGH_BITS = HEAP32[16775];
  return HEAP32[16774];
 }
 
 function ntp_clear() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  HEAP32[2936] = 16e6;
  HEAP32[2937] = 16e6;
  HEAP32[16848] = 0;
  HEAP32[16849] = 0;
  HEAP32[2938] = -1;
  HEAP32[2939] = 2147483647;
  HEAP32[2935] = HEAP32[2935] | 64;
  $1 = HEAP32[16853] + HEAP32[16857] | 0;
  $3 = HEAP32[16856];
  $0 = $3 + HEAP32[16852] | 0;
  if ($0 >>> 0 < $3 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $3 = 0;
  $4 = $3 + $0 | 0;
  $2 = Math_imul(HEAP32[2934], 1024e3) + $1 | 0;
  $2 = $4 >>> 0 < 0 ? $2 + 1 | 0 : $2;
  $3 = $2;
  $5 = __wasm_i64_mul($4, 0, -1924145349, 0);
  $0 = i64toi32_i32$HIGH_BITS;
  $6 = $0;
  $0 = $0 + -2095944041 | 0;
  $1 = $5 + -1924145349 | 0;
  if ($1 >>> 0 < 2370821947) {
   $0 = $0 + 1 | 0
  }
  $2 = $1;
  $1 = $0;
  $0 = ($6 | 0) == ($0 | 0) & $2 >>> 0 < $5 >>> 0 | $0 >>> 0 < $6 >>> 0;
  $7 = $1;
  $1 = $0;
  $5 = $3;
  $3 = 0;
  $0 = __wasm_i64_mul($5, $3, -1924145349, 0);
  $2 = $0 + $7 | 0;
  $1 = i64toi32_i32$HIGH_BITS + $1 | 0;
  $1 = $2 >>> 0 < $0 >>> 0 ? $1 + 1 | 0 : $1;
  $6 = $2;
  $2 = __wasm_i64_mul($4, 0, -2095944041, 0);
  $7 = $6 + $2 | 0;
  $4 = $1;
  $0 = $1 + i64toi32_i32$HIGH_BITS | 0;
  $0 = $7 >>> 0 < $2 >>> 0 ? $0 + 1 | 0 : $0;
  $2 = $7;
  $1 = $0;
  $0 = ($4 | 0) == ($0 | 0) & $2 >>> 0 < $6 >>> 0 | $0 >>> 0 < $4 >>> 0;
  $2 = $1;
  $1 = __wasm_i64_mul($5, 0, -2095944041, 0);
  $3 = $2 + $1 | 0;
  $2 = i64toi32_i32$HIGH_BITS + $0 | 0;
  $2 = $3 >>> 0 < $1 >>> 0 ? $2 + 1 | 0 : $2;
  $0 = $2;
  HEAP32[16854] = $0 >>> 7;
  $1 = $0 >>> 7 | 0;
  $0 = ($0 & 127) << 25 | $3 >>> 7;
  HEAP32[16844] = $0;
  HEAP32[16845] = $1;
  HEAP32[16846] = $0;
  HEAP32[16847] = $1;
  HEAP32[16842] = 0;
 }
 
 function ntp_get_next_leap() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0;
  $1 = -1;
  $0 = 2147483647;
  label$1 : {
   if (HEAP32[16850] != 1) {
    break label$1
   }
   if (!(HEAPU8[11740] & 16)) {
    break label$1
   }
   $1 = HEAP32[2938];
   $0 = HEAP32[2939];
   $2 = __wasm_i64_mul($1, $0, 1e9, 0);
   $3 = i64toi32_i32$HIGH_BITS;
   if (($0 | 0) > 2) {
    $0 = 1
   } else {
    if (($0 | 0) >= 2) {
     $0 = $1 >>> 0 <= 633437443 ? 0 : 1
    } else {
     $0 = 0
    }
   }
   $1 = $0 ? -1 : $2;
   $0 = $0 ? 2147483647 : $3;
  }
  i64toi32_i32$HIGH_BITS = $0;
  return $1;
 }
 
 function jiffies_read($0) {
  $0 = $0 | 0;
  i64toi32_i32$HIGH_BITS = 0;
  return HEAP32[20745];
 }
 
 function call_rcu($0, $1) {
  HEAP32[$0 >> 2] = 0;
  HEAP32[$0 + 4 >> 2] = $1;
  HEAP32[HEAP32[2966] >> 2] = $0;
  HEAP32[2966] = $0;
  if (!(HEAPU8[HEAP32[2] + 12 | 0] & 2)) {
   return
  }
  resched_cpu();
 }
 
 function rcu_process_callbacks($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  label$1 : {
   $1 = HEAP32[2965];
   if (($1 | 0) == 11856) {
    break label$1
   }
   $0 = HEAP32[2964];
   HEAP32[2964] = HEAP32[$1 >> 2];
   HEAP32[$1 >> 2] = 0;
   if (HEAP32[2966] == HEAP32[2965]) {
    HEAP32[2966] = 11856
   }
   HEAP32[2965] = 11856;
   if (!$0) {
    break label$1
   }
   while (1) {
    HEAP32[1] = HEAP32[1] + 512;
    $1 = HEAP32[$0 >> 2];
    $2 = HEAP32[$0 + 4 >> 2];
    if ($2 >>> 0 <= 4095) {
     kfree($0 - $2 | 0);
     $0 = $1;
     if ($0) {
      continue
     }
     break label$1;
    }
    HEAP32[$0 + 4 >> 2] = 0;
    FUNCTION_TABLE[$2]($0);
    $0 = $1;
    if ($0) {
     continue
    }
    break;
   };
  }
 }
 
 function msg_print_ext_body($0, $1, $2, $3, $4, $5) {
  var $6 = 0, $7 = 0, $8 = 0;
  $7 = global$0 - 32 | 0;
  global$0 = $7;
  $6 = $0 + $1 | 0;
  $1 = $0;
  if ($5) {
   while (1) {
    label$3 : {
     label$4 : {
      $8 = HEAPU8[$4 | 0];
      if (($8 | 0) == 92) {
       break label$4
      }
      if (($8 + -32 & 255) >>> 0 >= 95) {
       break label$4
      }
      if ($6 >>> 0 <= $1 >>> 0) {
       break label$3
      }
      HEAP8[$1 | 0] = $8;
      $1 = $1 + 1 | 0;
      break label$3;
     }
     HEAP32[$7 + 16 >> 2] = $8;
     $1 = scnprintf($1, $6 - $1 | 0, 22073, $7 + 16 | 0) + $1 | 0;
    }
    $4 = $4 + 1 | 0;
    $5 = $5 + -1 | 0;
    if ($5) {
     continue
    }
    break;
   }
  }
  if ($6 >>> 0 > $1 >>> 0) {
   HEAP8[$1 | 0] = 10;
   $1 = $1 + 1 | 0;
  }
  label$6 : {
   if (!$3) {
    break label$6
   }
   $4 = 1;
   while (1) {
    $5 = HEAPU8[$2 | 0];
    label$8 : {
     if (!$4) {
      break label$8
     }
     if ($6 >>> 0 <= $1 >>> 0) {
      break label$8
     }
     HEAP8[$1 | 0] = 32;
     $1 = $1 + 1 | 0;
    }
    label$9 : {
     label$10 : {
      if ($5) {
       if (($5 | 0) == 92) {
        break label$10
       }
       if (($5 + -32 & 255) >>> 0 >= 95) {
        break label$10
       }
       $4 = 0;
       if ($6 >>> 0 <= $1 >>> 0) {
        break label$9
       }
       HEAP8[$1 | 0] = $5;
       $1 = $1 + 1 | 0;
       break label$9;
      }
      $4 = 1;
      if ($6 >>> 0 <= $1 >>> 0) {
       break label$9
      }
      HEAP8[$1 | 0] = 10;
      $1 = $1 + 1 | 0;
      break label$9;
     }
     HEAP32[$7 >> 2] = $5;
     $1 = scnprintf($1, $6 - $1 | 0, 22073, $7) + $1 | 0;
     $4 = 0;
    }
    $2 = $2 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
   if ($6 >>> 0 <= $1 >>> 0) {
    break label$6
   }
   HEAP8[$1 | 0] = 10;
   $1 = $1 + 1 | 0;
  }
  global$0 = $7 + 32 | 0;
  return $1 - $0 | 0;
 }
 
 function printk($0, $1) {
  var $2 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  HEAP32[$2 + 12 >> 2] = $1;
  vprintk_func($0, $1);
  global$0 = $2 + 16 | 0;
 }
 
 function setup_log_buf() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0;
  $0 = global$0 - 48 | 0;
  global$0 = $0;
  label$1 : {
   if (HEAP32[2974] != 67432) {
    break label$1
   }
   $1 = HEAP32[35290];
   if (!$1) {
    break label$1
   }
   $1 = memblock_alloc_try_nid_nopanic($1, 8, -1);
   if ($1) {
    $2 = HEAPU8[78268];
    HEAP8[78268] = 0;
    HEAP32[3049] = HEAP32[3049] + 1;
    HEAP32[2974] = $1;
    HEAP32[2975] = HEAP32[35290];
    HEAP32[35290] = 0;
    $3 = HEAP32[16986];
    memcpy($1, 67432, 512);
    HEAP32[3049] = HEAP32[3049] + -1;
    HEAP8[78268] = $2 & 1;
    HEAP32[$0 + 32 >> 2] = HEAP32[2975];
    printk(21901, $0 + 32 | 0);
    $1 = 512 - $3 | 0;
    HEAP32[$0 + 16 >> 2] = $1;
    HEAP32[$0 + 20 >> 2] = Math_imul($1, 100) >>> 9;
    printk(21933, $0 + 16 | 0);
    break label$1;
   }
   HEAP32[$0 >> 2] = HEAP32[35290];
   printk(21854, $0);
  }
  global$0 = $0 + 48 | 0;
 }
 
 function msg_print_text($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0;
  $10 = global$0 - 48 | 0;
  global$0 = $10;
  $8 = $0 + 16 | 0;
  $15 = HEAPU16[$0 + 10 >> 1];
  $19 = $0 + 15 | 0;
  $20 = $0 + 14 | 0;
  while (1) {
   $4 = memchr($8, $15);
   label$2 : {
    if ($4) {
     $17 = $4 + 1 | 0;
     $15 = ($8 + $15 | 0) - $17 | 0;
     $5 = $4 - $8 | 0;
     break label$2;
    }
    $17 = 0;
    $5 = $15;
   }
   $18 = $5;
   $5 = 0;
   label$4 : {
    if (!$1) {
     break label$4
    }
    $6 = HEAPU8[$20 | 0] << 3 | HEAPU8[$19 | 0] >>> 5;
    $5 = 6;
    if ($6 >>> 0 > 999) {
     break label$4
    }
    $5 = 5;
    if ($6 >>> 0 > 99) {
     break label$4
    }
    $5 = $6 >>> 0 > 9 ? 4 : 3;
   }
   $13 = $5;
   if (HEAPU8[67949]) {
    $5 = HEAP32[$0 + 4 >> 2];
    $9 = HEAP32[$0 >> 2];
    $6 = __wasm_i64_mul($9, 0, 917808535, 0) + 917808535 | 0;
    $4 = i64toi32_i32$HIGH_BITS + -1989124287 | 0;
    $16 = $5;
    $7 = 0;
    $11 = __wasm_i64_mul($5, $7, 917808535, 0);
    $5 = $11 + ($6 >>> 0 < 917808535 ? $4 + 1 | 0 : $4) | 0;
    $4 = i64toi32_i32$HIGH_BITS;
    $4 = $5 >>> 0 < $11 >>> 0 ? $4 + 1 | 0 : $4;
    $11 = $5;
    $9 = __wasm_i64_mul($9, 0, -1989124287, 0);
    $6 = $5 + $9 | 0;
    $5 = $4;
    $4 = $4 + i64toi32_i32$HIGH_BITS | 0;
    $4 = $6 >>> 0 < $9 >>> 0 ? $4 + 1 | 0 : $4;
    $7 = $6;
    $6 = $4;
    $4 = ($5 | 0) == ($4 | 0) & $7 >>> 0 < $11 >>> 0 | $4 >>> 0 < $5 >>> 0;
    $7 = __wasm_i64_mul($16, 0, -1989124287, 0);
    $6 = $7 + $6 | 0;
    $4 = i64toi32_i32$HIGH_BITS + $4 | 0;
    $4 = $6 >>> 0 < $7 >>> 0 ? $4 + 1 | 0 : $4;
    HEAP32[$10 + 32 >> 2] = ($4 & 536870911) << 3 | $6 >>> 29;
    $5 = snprintf(0, 0, 22013, $10 + 32 | 0);
   } else {
    $5 = 0
   }
   $4 = ($5 + $13 | 0) + $18 | 0;
   label$6 : {
    label$7 : {
     if ($2) {
      if ($4 + 1 >>> 0 >= $3 - $14 >>> 0) {
       break label$6
      }
      $16 = $2 + $14 | 0;
      $13 = 0;
      if ($1) {
       HEAP32[$10 + 16 >> 2] = HEAPU8[$20 | 0] << 3 | HEAPU8[$19 | 0] >>> 5;
       $13 = sprintf($16, 22008, $10 + 16 | 0);
      }
      if (HEAPU8[67949]) {
       $5 = HEAP32[$0 + 4 >> 2];
       $9 = HEAP32[$0 >> 2];
       $4 = __wasm_i64_mul($9, 0, 917808535, 0) + 917808535 | 0;
       $6 = i64toi32_i32$HIGH_BITS + -1989124287 | 0;
       $12 = $4 >>> 0 < 917808535 ? $6 + 1 | 0 : $6;
       $4 = $12;
       $11 = $5;
       $7 = 0;
       $12 = __wasm_i64_mul($5, $7, 917808535, 0);
       $5 = $4 + $12 | 0;
       $4 = i64toi32_i32$HIGH_BITS;
       $4 = $5 >>> 0 < $12 >>> 0 ? $4 + 1 | 0 : $4;
       $12 = $5;
       $7 = __wasm_i64_mul($9, 0, -1989124287, 0);
       $6 = $5 + $7 | 0;
       $5 = $4;
       $4 = $4 + i64toi32_i32$HIGH_BITS | 0;
       $4 = $6 >>> 0 < $7 >>> 0 ? $4 + 1 | 0 : $4;
       $7 = $6;
       $6 = $4;
       $4 = ($5 | 0) == ($4 | 0) & $7 >>> 0 < $12 >>> 0 | $4 >>> 0 < $5 >>> 0;
       $11 = __wasm_i64_mul($11, 0, -1989124287, 0);
       $7 = $11 + $6 | 0;
       $6 = i64toi32_i32$HIGH_BITS + $4 | 0;
       $4 = $7;
       $4 = (($4 >>> 0 < $11 >>> 0 ? $6 + 1 | 0 : $6) & 536870911) << 3 | $4 >>> 29;
       HEAP32[$10 >> 2] = $4;
       HEAP32[$10 + 4 >> 2] = ($9 + Math_imul($4, -1e9) >>> 0) / 1e3;
       $5 = sprintf($13 + $16 | 0, 22028, $10);
      } else {
       $5 = 0
      }
      $4 = $5 + ($14 + $13 | 0) | 0;
      memcpy($4 + $2 | 0, $8, $18);
      $8 = $4 + $18 | 0;
      HEAP8[$8 + $2 | 0] = 10;
      break label$7;
     }
     $8 = $4 + $14 | 0;
    }
    $14 = $8 + 1 | 0;
    $8 = $17;
    if ($8) {
     continue
    }
   }
   break;
  };
  global$0 = $10 + 48 | 0;
  return $14;
 }
 
 function vprintk_store($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $5 = -1;
  $3 = 67984;
  $0 = vscnprintf(67984, 992, $0, $1);
  if ($0) {
   $2 = $0 + -1 | 0;
   $1 = $0;
   $0 = HEAPU8[$0 + 67983 | 0] == 10;
   $2 = $0 ? $2 : $1;
   $4 = $0 << 1;
  }
  label$2 : {
   if (HEAPU8[67984] != 1) {
    break label$2
   }
   while (1) {
    label$4 : {
     label$5 : {
      $0 = $3;
      $3 = HEAP8[$0 + 1 | 0];
      if (!$3) {
       break label$5
      }
      $1 = 4;
      $6 = $3 + -48 | 0;
      if ($6 >>> 0 < 8) {
       $5 = ($5 | 0) == -1 ? $6 : $5;
       break label$4;
      }
      if (($3 | 0) != 99) {
       if (($3 | 0) == 100) {
        break label$4
       }
       break label$5;
      }
      $1 = 8;
      break label$4;
     }
     $3 = $0;
     break label$2;
    }
    $3 = $0 + 2 | 0;
    $2 = $2 + -2 | 0;
    $4 = $1 | $4;
    if (HEAPU8[$0 + 2 | 0] == 1) {
     continue
    }
    break;
   };
  }
  $6 = HEAP32[2969];
  label$8 : {
   label$9 : {
    label$10 : {
     label$11 : {
      $0 = HEAP32[19816];
      if ($0) {
       $1 = $4 & 8;
       if (!$1) {
        break label$11
       }
       if (HEAP32[19817] != HEAP32[2]) {
        break label$11
       }
       if ($0 + $2 >>> 0 <= 992) {
        break label$10
       }
       log_store(HEAPU8[79281], HEAPU8[79280], HEAP32[19821], HEAP32[19818], HEAP32[19819], 78272, $0 & 65535);
       HEAP32[19816] = 0;
       if (!$2) {
        break label$9
       }
       break label$8;
      }
      $1 = $4 & 8;
      if ($2) {
       break label$8
      }
      break label$9;
     }
     log_store(HEAPU8[79281], HEAPU8[79280], HEAP32[19821], HEAP32[19818], HEAP32[19819], 78272, $0 & 65535);
     HEAP32[19816] = 0;
     if (!$2) {
      break label$9
     }
     break label$8;
    }
    memcpy($0 + 78272 | 0, $3, $2);
    $0 = HEAP32[19816] + $2 | 0;
    HEAP32[19816] = $0;
    if (!($4 & 2)) {
     return
    }
    $1 = HEAP32[19821] | 2;
    HEAP32[19821] = $1;
    if ($0) {
     log_store(HEAPU8[79281], HEAPU8[79280], $1, HEAP32[19818], HEAP32[19819], 78272, $0 & 65535);
     HEAP32[19816] = 0;
     return;
    }
    return;
   }
   if (!$1) {
    break label$8
   }
   return;
  }
  $0 = ($5 | 0) == -1 ? $6 : $5;
  label$15 : {
   if ($2 >>> 0 > 992) {
    break label$15
   }
   if ($4 & 2) {
    break label$15
   }
   HEAP8[79280] = $0;
   HEAP8[79281] = 0;
   HEAP32[19817] = HEAP32[2];
   $0 = sched_clock();
   HEAP32[19821] = $4;
   HEAP32[19818] = $0;
   HEAP32[19819] = i64toi32_i32$HIGH_BITS;
   memcpy(HEAP32[19816] + 78272 | 0, $3, $2);
   HEAP32[19816] = HEAP32[19816] + $2;
   return;
  }
  log_store(0, $0, $4, 0, 0, $3, $2 & 65535);
 }
 
 function log_store($0, $1, $2, $3, $4, $5, $6) {
  var $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0;
  $16 = -16 - $6 & 7;
  $13 = ($6 + $16 | 0) + 16 | 0;
  $8 = HEAP32[16991];
  $11 = HEAP32[16989];
  $7 = $11;
  $10 = HEAP32[16990];
  $12 = HEAP32[16988];
  label$1 : {
   if (($8 | 0) == ($7 | 0) & $10 >>> 0 >= $12 >>> 0 | $8 >>> 0 > $7 >>> 0) {
    break label$1
   }
   $17 = $13 + 16 | 0;
   $9 = HEAP32[2974] + 8 | 0;
   $14 = HEAP32[16986];
   $15 = HEAP32[2975] - $14 | 0;
   $7 = HEAP32[16992];
   while (1) {
    if (($7 >>> 0 < $14 >>> 0 ? ($15 >>> 0 > $7 >>> 0 ? $15 : $7) : $7 - $14 | 0) >>> 0 >= $17 >>> 0) {
     break label$1
    }
    $18 = HEAPU16[$7 + $9 >> 1];
    label$3 : {
     if ($18) {
      $7 = $7 + $18 | 0;
      break label$3;
     }
     $7 = HEAPU16[$9 >> 1];
    }
    HEAP32[16992] = $7;
    $10 = $10 + 1 | 0;
    if ($10 >>> 0 < 1) {
     $8 = $8 + 1 | 0
    }
    HEAP32[16990] = $10;
    HEAP32[16991] = $8;
    if (($8 | 0) == ($11 | 0) & $10 >>> 0 < $12 >>> 0 | $8 >>> 0 < $11 >>> 0) {
     continue
    }
    break;
   };
  }
  $7 = HEAP32[19565];
  label$5 : {
   if (!(($8 | 0) == ($7 | 0) & HEAPU32[19564] >= $10 >>> 0 | $7 >>> 0 > $8 >>> 0)) {
    HEAP32[19564] = $10;
    HEAP32[19565] = $8;
    $9 = HEAP32[16992];
    HEAP32[19566] = $9;
    break label$5;
   }
   $9 = HEAP32[16992];
  }
  $7 = HEAP32[16986];
  label$9 : {
   label$10 : {
    label$11 : {
     label$12 : {
      label$7 : {
       label$8 : {
        if (($10 | 0) == ($12 | 0) & ($8 | 0) == ($11 | 0)) {
         break label$8
        }
        if ($9 >>> 0 < $7 >>> 0) {
         break label$8
        }
        $8 = $9 - $7 | 0;
        break label$7;
       }
       $8 = HEAP32[2975] - $7 | 0;
       $8 = $8 >>> 0 > $9 >>> 0 ? $8 : $9;
      }
      $9 = $13 + 16 | 0;
      if ($8 >>> 0 < $9 >>> 0) {
       $7 = HEAP32[2975] >>> 2 | 0;
       $6 = $7 >>> 0 < $6 >>> 0 ? $7 : $6;
       $11 = strlen(22042);
       $7 = $6 + $11 | 0;
       $16 = 0 - $7 & 7;
       $13 = ($16 + ($7 & 65535) | 0) + 16 | 0;
       $8 = HEAP32[16991];
       $7 = $8;
       $12 = HEAP32[16989];
       $9 = $12;
       $10 = HEAP32[16990];
       $14 = HEAP32[16988];
       label$14 : {
        if (($7 | 0) == ($9 | 0) & $10 >>> 0 >= $14 >>> 0 | $7 >>> 0 > $9 >>> 0) {
         break label$14
        }
        $18 = $13 + 16 | 0;
        $9 = HEAP32[2974] + 8 | 0;
        $15 = HEAP32[16986];
        $17 = HEAP32[2975] - $15 | 0;
        $7 = HEAP32[16992];
        while (1) {
         if (($7 >>> 0 < $15 >>> 0 ? ($17 >>> 0 > $7 >>> 0 ? $17 : $7) : $7 - $15 | 0) >>> 0 >= $18 >>> 0) {
          break label$14
         }
         $19 = HEAPU16[$7 + $9 >> 1];
         label$16 : {
          if ($19) {
           $7 = $7 + $19 | 0;
           break label$16;
          }
          $7 = HEAPU16[$9 >> 1];
         }
         HEAP32[16992] = $7;
         $10 = $10 + 1 | 0;
         if ($10 >>> 0 < 1) {
          $8 = $8 + 1 | 0
         }
         HEAP32[16990] = $10;
         HEAP32[16991] = $8;
         if (($8 | 0) == ($12 | 0) & $10 >>> 0 < $14 >>> 0 | $8 >>> 0 < $12 >>> 0) {
          continue
         }
         break;
        };
       }
       $7 = HEAP32[19565];
       if (($8 | 0) == ($7 | 0) & HEAPU32[19564] >= $10 >>> 0 | $7 >>> 0 > $8 >>> 0) {
        break label$12
       }
       HEAP32[19564] = $10;
       HEAP32[19565] = $8;
       $9 = HEAP32[16992];
       HEAP32[19566] = $9;
       break label$11;
      }
      $11 = 0;
      break label$10;
     }
     $9 = HEAP32[16992];
    }
    $7 = HEAP32[16986];
    label$18 : {
     label$19 : {
      if (($10 | 0) == ($14 | 0) & ($8 | 0) == ($12 | 0)) {
       break label$19
      }
      if ($9 >>> 0 < $7 >>> 0) {
       break label$19
      }
      $8 = $9 - $7 | 0;
      break label$18;
     }
     $8 = HEAP32[2975] - $7 | 0;
     $8 = $8 >>> 0 > $9 >>> 0 ? $8 : $9;
    }
    $9 = $13 + 16 | 0;
    if ($8 >>> 0 < $9 >>> 0) {
     break label$9
    }
   }
   label$20 : {
    if ($7 + $9 >>> 0 > HEAPU32[2975]) {
     memset(HEAP32[2974] + $7 | 0, 0, 16);
     HEAP32[16986] = 0;
     $7 = 0;
     break label$20;
    }
   }
   $7 = $7 + HEAP32[2974] | 0;
   $8 = $7 + 16 | 0;
   $10 = $6 & 65535;
   memcpy($8, $5, $10);
   HEAP16[$7 + 10 >> 1] = $6;
   $6 = $11 & 65535;
   if ($6) {
    memcpy($8 + $10 | 0, 22042, $6);
    $9 = $7 + 10 | 0;
    $6 = HEAPU16[$9 >> 1] + $11 | 0;
    HEAP16[$9 >> 1] = $6;
    $6 = $6 & 65535;
   } else {
    $6 = $10
   }
   memcpy($6 + $8 | 0, 0, 0);
   HEAP8[$7 + 15 | 0] = $2 & 31 | $1 << 5;
   HEAP8[$7 + 14 | 0] = $0;
   HEAP16[$7 + 12 >> 1] = 0;
   if (!(($3 | 0) != 0 | ($4 | 0) != 0)) {
    $3 = sched_clock();
    $4 = i64toi32_i32$HIGH_BITS;
   }
   HEAP32[$7 >> 2] = $3;
   HEAP32[$7 + 4 >> 2] = $4;
   memset(HEAPU16[$7 + 10 >> 1] + $8 | 0, 0, $16);
   HEAP16[$7 + 8 >> 1] = $13;
   HEAP32[16986] = HEAP32[16986] + ($13 & 65535);
   $0 = HEAP32[16989];
   $2 = HEAP32[16988] + 1 | 0;
   if ($2 >>> 0 < 1) {
    $0 = $0 + 1 | 0
   }
   HEAP32[16988] = $2;
   HEAP32[16989] = $0;
   write(1, $5, $10);
   write(1, 22054, 1);
  }
 }
 
 function vprintk_emit($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $2 = HEAPU8[78268];
  HEAP8[78268] = 0;
  HEAP32[3049] = HEAP32[3049] + 1;
  $3 = HEAP32[16988];
  $4 = HEAP32[16989];
  vprintk_store($0, $1);
  $1 = HEAP32[16988];
  $5 = HEAP32[16989];
  HEAP32[3049] = HEAP32[3049] + -1;
  HEAP8[78268] = $2 & 1;
  if (!(($1 | 0) == ($3 | 0) & ($4 | 0) == ($5 | 0))) {
   $0 = HEAPU8[78268];
   HEAP8[78268] = 0;
   HEAP32[3049] = HEAP32[3049] + 1;
   $2 = down_trylock();
   HEAP32[3049] = HEAP32[3049] + -1;
   HEAP8[78268] = $0 & 1;
   label$2 : {
    label$3 : {
     label$4 : {
      if (!$2) {
       if (!HEAPU8[68976]) {
        break label$4
       }
       HEAP8[78268] = 0;
       HEAP32[3049] = HEAP32[3049] + 1;
       up();
       HEAP32[3049] = HEAP32[3049] + -1;
       HEAP8[78268] = $0 & 1;
      }
      HEAP8[78268] = 0;
      HEAP32[3049] = HEAP32[3049] + 1;
      $2 = HEAPU8[79292];
      label$6 : {
       $6 = HEAP32[19822];
       if (!$6) {
        break label$6
       }
       if ($2) {
        break label$6
       }
       if (($6 | 0) == HEAP32[2]) {
        break label$6
       }
       HEAP8[79292] = 1;
       while (1) {
        if (HEAPU8[79292]) {
         continue
        }
        break;
       };
       HEAP32[3049] = HEAP32[3049] + -1;
       HEAP8[78268] = $0 & 1;
       break label$3;
      }
      HEAP32[3049] = HEAP32[3049] + -1;
      HEAP8[78268] = $0 & 1;
      break label$2;
     }
     HEAP8[68980] = 1;
     HEAP8[68984] = 0;
    }
    console_unlock();
   }
  }
  if (!(($1 | 0) == ($3 | 0) & ($4 | 0) == ($5 | 0))) {
   if (HEAP32[2972] != 11888) {
    HEAP32[2979] = HEAP32[2979] | 1;
    irq_work_queue(11920);
   }
  }
 }
 
 function console_unlock() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $5 = global$0 - 48 | 0;
  global$0 = $5;
  label$1 : {
   if (HEAPU8[68976]) {
    $0 = HEAPU8[78268];
    HEAP8[78268] = 0;
    HEAP32[3049] = HEAP32[3049] + 1;
    up();
    HEAP32[3049] = HEAP32[3049] + -1;
    HEAP8[78268] = $0 & 1;
    break label$1;
   }
   $15 = HEAPU8[68984] ^ 1;
   $16 = $5 + 24 | 0;
   label$3 : while (1) {
    HEAP8[68984] = 0;
    while (1) {
     $7 = 0;
     $11 = HEAPU8[78268];
     HEAP8[78268] = 0;
     HEAP32[3049] = HEAP32[3049] + 1;
     $3 = HEAP32[16991];
     $1 = HEAP32[19553];
     $0 = HEAP32[16990];
     $2 = HEAP32[19552];
     if (!(($3 | 0) == ($1 | 0) & $0 >>> 0 <= $2 >>> 0 | $3 >>> 0 < $1 >>> 0)) {
      HEAP32[$5 + 32 >> 2] = $0 - $2;
      HEAP32[$5 + 36 >> 2] = $3 - ($1 + ($0 >>> 0 < $2 >>> 0) | 0);
      $7 = sprintf(77184, 21972, $5 + 32 | 0);
      $0 = HEAP32[16991];
      $1 = $0;
      $2 = HEAP32[16990];
      HEAP32[19552] = $2;
      HEAP32[19553] = $0;
      HEAP32[19554] = HEAP32[16992];
     }
     label$6 : {
      label$7 : {
       $8 = HEAP32[16988];
       $9 = HEAP32[16989];
       if (($8 | 0) == ($2 | 0) & ($9 | 0) == ($1 | 0)) {
        break label$7
       }
       $0 = HEAP32[19554];
       $12 = HEAPU8[67948];
       $10 = HEAP32[2968];
       $6 = HEAP32[2974];
       while (1) {
        label$8 : {
         $3 = $0 + $6 | 0;
         $4 = HEAPU16[$3 + 8 >> 1];
         $3 = $4 ? $3 : $6;
         if (($10 | 0) > (HEAPU8[$3 + 15 | 0] >>> 5 | 0)) {
          break label$8
         }
         if ($12) {
          break label$8
         }
         $0 = $4 ? $0 + $4 | 0 : HEAPU16[$6 + 8 >> 1];
         HEAP32[19554] = $0;
         $2 = $2 + 1 | 0;
         if ($2 >>> 0 < 1) {
          $1 = $1 + 1 | 0
         }
         HEAP32[19552] = $2;
         HEAP32[19553] = $1;
         if (($2 | 0) != ($8 | 0) | ($1 | 0) != ($9 | 0)) {
          continue
         }
         break label$7;
        }
        break;
       };
       if (!HEAP32[19555]) {
        break label$6
       }
       $0 = HEAP32[19557];
       if (($0 | 0) == ($1 | 0) & $2 >>> 0 < HEAPU32[19556] | $1 >>> 0 < $0 >>> 0) {
        break label$6
       }
       HEAP32[19555] = 0;
       break label$6;
      }
      HEAP8[68980] = 0;
      $0 = HEAPU8[78268];
      HEAP8[78268] = 0;
      HEAP32[3049] = HEAP32[3049] + 1;
      up();
      HEAP32[3049] = HEAP32[3049] + -1;
      HEAP8[78268] = $0 & 1;
      $0 = HEAP32[16988];
      $1 = HEAP32[16989];
      $2 = HEAP32[19552];
      $3 = HEAP32[19553];
      HEAP32[3049] = HEAP32[3049] + -1;
      $7 = $11 & 1;
      HEAP8[78268] = $7;
      if (($0 | 0) == ($2 | 0) & ($1 | 0) == ($3 | 0)) {
       break label$1
      }
      HEAP8[78268] = 0;
      HEAP32[3049] = HEAP32[3049] + 1;
      $0 = down_trylock();
      HEAP32[3049] = HEAP32[3049] + -1;
      HEAP8[78268] = $7;
      if ($0) {
       break label$1
      }
      if (!HEAPU8[68976]) {
       HEAP8[68980] = 1;
       HEAP8[68984] = 0;
       continue label$3;
      }
      HEAP8[78268] = 0;
      HEAP32[3049] = HEAP32[3049] + 1;
      up();
      HEAP32[3049] = HEAP32[3049] + -1;
      HEAP8[78268] = $11 & 1;
      break label$1;
     }
     $1 = 0;
     $9 = msg_print_text($3, HEAPU8[78232], $7 + 77184 | 0, 1024 - $7 | 0);
     if (HEAP32[19559]) {
      $12 = HEAPU8[$3 + 14 | 0];
      $2 = HEAP32[$3 >> 2];
      $6 = HEAP32[$3 + 4 >> 2];
      $10 = HEAPU8[$3 + 15 | 0];
      HEAP32[$16 >> 2] = $10 & 8 ? 99 : 45;
      $4 = __wasm_i64_mul($2, 0, -1924145349, 0);
      $0 = i64toi32_i32$HIGH_BITS;
      $8 = $0;
      $1 = $0 + -2095944041 | 0;
      $13 = $4 + -1924145349 | 0;
      if ($13 >>> 0 < 2370821947) {
       $1 = $1 + 1 | 0
      }
      $14 = __wasm_i64_mul($6, 0, -1924145349, 0);
      $0 = $14 + $1 | 0;
      $1 = i64toi32_i32$HIGH_BITS + (($1 | 0) == ($8 | 0) & $13 >>> 0 < $4 >>> 0 | $1 >>> 0 < $8 >>> 0) | 0;
      $1 = $0 >>> 0 < $14 >>> 0 ? $1 + 1 | 0 : $1;
      $4 = __wasm_i64_mul($2, 0, -2095944041, 0);
      $2 = $4 + $0 | 0;
      $8 = i64toi32_i32$HIGH_BITS + $1 | 0;
      $4 = $2 >>> 0 < $4 >>> 0 ? $8 + 1 | 0 : $8;
      $1 = ($1 | 0) == ($4 | 0) & $2 >>> 0 < $0 >>> 0 | $4 >>> 0 < $1 >>> 0;
      $2 = __wasm_i64_mul($6, 0, -2095944041, 0);
      $0 = $4 + $2 | 0;
      $6 = $5 + 16 | 0;
      $1 = i64toi32_i32$HIGH_BITS + $1 | 0;
      $1 = $0 >>> 0 < $2 >>> 0 ? $1 + 1 | 0 : $1;
      HEAP32[$6 >> 2] = ($1 & 511) << 23 | $0 >>> 9;
      HEAP32[$6 + 4 >> 2] = $1 >>> 9;
      $0 = HEAP32[19553];
      HEAP32[$5 + 8 >> 2] = HEAP32[19552];
      HEAP32[$5 + 12 >> 2] = $0;
      HEAP32[$5 >> 2] = $12 << 3 | $10 >>> 5;
      $0 = scnprintf(68992, 8192, 22056, $5);
      $4 = $0;
      $8 = $0 + 68992 | 0;
      $6 = 8192 - $0 | 0;
      $0 = $3 + 16 | 0;
      $1 = HEAPU16[$3 + 10 >> 1];
      $1 = $4 + msg_print_ext_body($8, $6, $0 + $1 | 0, HEAPU16[$3 + 12 >> 1], $0, $1) | 0;
     }
     $0 = HEAP32[2974] + 8 | 0;
     $2 = HEAP32[19554];
     $3 = HEAPU16[$0 + $2 >> 1];
     label$14 : {
      if ($3) {
       $0 = $2 + $3 | 0;
       break label$14;
      }
      $0 = HEAPU16[$0 >> 1];
     }
     HEAP32[19554] = $0;
     $2 = HEAP32[19553];
     $0 = HEAP32[19552] + 1 | 0;
     if ($0 >>> 0 < 1) {
      $2 = $2 + 1 | 0
     }
     HEAP32[19552] = $0;
     HEAP32[19553] = $2;
     HEAP32[19822] = HEAP32[2];
     $0 = HEAP32[19561];
     label$16 : {
      if (!$0) {
       break label$16
      }
      $3 = $7 + $9 | 0;
      while (1) {
       label$18 : {
        $2 = HEAP32[19555];
        label$19 : {
         if (($0 | 0) != ($2 | 0) ? $2 : 0) {
          break label$19
         }
         $7 = HEAP16[$0 + 40 >> 1];
         if (!($7 & 4)) {
          break label$19
         }
         $2 = HEAP32[$0 + 16 >> 2];
         if (!$2) {
          break label$19
         }
         if ($7 & 64) {
          break label$18
         }
         FUNCTION_TABLE[$2]($0, 77184, $3);
        }
        $0 = HEAP32[$0 + 52 >> 2];
        if ($0) {
         continue
        }
        break label$16;
       }
       FUNCTION_TABLE[$2]($0, 68992, $1);
       $0 = HEAP32[$0 + 52 >> 2];
       if ($0) {
        continue
       }
       break;
      };
     }
     $0 = 0;
     HEAP32[19822] = 0;
     if (HEAPU8[79292]) {
      HEAP8[79292] = 0;
      $0 = 1;
     }
     HEAP32[3049] = HEAP32[3049] + -1;
     HEAP8[78268] = $11 & 1;
     if (!(($0 | $15) & 1)) {
      _cond_resched();
      $0 = 0;
     }
     if (!$0) {
      continue
     }
     break;
    };
    break;
   };
  }
  global$0 = $5 + 48 | 0;
 }
 
 function console_unblank() {
  var $0 = 0, $1 = 0;
  label$1 : {
   if (HEAP32[19560]) {
    $0 = HEAPU8[78268];
    HEAP8[78268] = 0;
    HEAP32[3049] = HEAP32[3049] + 1;
    $1 = down_trylock();
    HEAP32[3049] = HEAP32[3049] + -1;
    HEAP8[78268] = $0 & 1;
    if (!$1) {
     break label$1
    }
    return;
   }
   down();
   if (HEAPU8[68976]) {
    break label$1
   }
   HEAP8[68984] = 1;
   HEAP8[68980] = 1;
  }
  HEAP8[68980] = 1;
  HEAP8[68984] = 0;
  $0 = HEAP32[19561];
  if ($0) {
   while (1) {
    label$5 : {
     if (!(HEAPU8[$0 + 40 | 0] & 4)) {
      break label$5
     }
     $1 = HEAP32[$0 + 28 >> 2];
     if (!$1) {
      break label$5
     }
     FUNCTION_TABLE[$1]();
    }
    $0 = HEAP32[$0 + 52 >> 2];
    if ($0) {
     continue
    }
    break;
   }
  }
  console_unlock();
 }
 
 function console_flush_on_panic() {
  var $0 = 0, $1 = 0;
  $0 = HEAPU8[78268];
  HEAP8[78268] = 0;
  HEAP32[3049] = HEAP32[3049] + 1;
  $1 = down_trylock();
  HEAP32[3049] = HEAP32[3049] + -1;
  HEAP8[78268] = $0 & 1;
  label$1 : {
   if ($1) {
    break label$1
   }
   if (HEAPU8[68976]) {
    HEAP8[78268] = 0;
    HEAP32[3049] = HEAP32[3049] + 1;
    up();
    HEAP32[3049] = HEAP32[3049] + -1;
    HEAP8[78268] = $0 & 1;
    break label$1;
   }
   HEAP8[68980] = 1;
   HEAP8[68984] = 0;
  }
  HEAP8[68984] = 0;
  console_unlock();
 }
 
 function vprintk_deferred($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $2 = HEAPU8[78268];
  HEAP8[78268] = 0;
  HEAP32[3049] = HEAP32[3049] + 1;
  $3 = HEAP32[16988];
  $4 = HEAP32[16989];
  vprintk_store($0, $1);
  $0 = HEAP32[16988];
  $1 = HEAP32[16989];
  HEAP32[3049] = HEAP32[3049] + -1;
  HEAP8[78268] = $2 & 1;
  if (!(($0 | 0) == ($3 | 0) & ($1 | 0) == ($4 | 0))) {
   if (HEAP32[2972] != 11888) {
    HEAP32[2979] = HEAP32[2979] | 1;
    irq_work_queue(11920);
   }
  }
  HEAP32[2979] = HEAP32[2979] | 2;
  irq_work_queue(11920);
 }
 
 function printk_deferred($0, $1) {
  var $2 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  HEAP32[$2 + 12 >> 2] = $1;
  vprintk_deferred($0, $1);
  global$0 = $2 + 16 | 0;
 }
 
 function kmsg_dump($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  label$1 : {
   if (HEAPU8[78248] ? 0 : $0 >>> 0 >= 3) {
    break label$1
   }
   $1 = HEAP32[2983];
   if (($1 | 0) == 11932) {
    break label$1
   }
   while (1) {
    $2 = HEAP32[$1 + 12 >> 2];
    if (!($2 >>> 0 < $0 >>> 0 ? !!$2 : 0)) {
     HEAP8[$1 + 16 | 0] = 1;
     $2 = HEAPU8[78268];
     HEAP8[78268] = 0;
     HEAP32[3049] = HEAP32[3049] + 1;
     $3 = HEAP32[19565];
     HEAP32[$1 + 32 >> 2] = HEAP32[19564];
     HEAP32[$1 + 36 >> 2] = $3;
     HEAP32[$1 + 20 >> 2] = HEAP32[19566];
     $3 = HEAP32[16989];
     HEAP32[$1 + 40 >> 2] = HEAP32[16988];
     HEAP32[$1 + 44 >> 2] = $3;
     HEAP32[$1 + 24 >> 2] = HEAP32[16986];
     HEAP32[3049] = HEAP32[3049] + -1;
     HEAP8[78268] = $2 & 1;
     FUNCTION_TABLE[HEAP32[$1 + 8 >> 2]]($1, $0);
     HEAP8[$1 + 16 | 0] = 0;
    }
    $1 = HEAP32[$1 >> 2];
    if (($1 | 0) != 11932) {
     continue
    }
    break;
   };
  }
 }
 
 function wake_up_klogd_work_func($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $0 = HEAP32[2979];
  HEAP32[2979] = 0;
  label$1 : {
   if (!($0 & 2)) {
    break label$1
   }
   $1 = HEAPU8[78268];
   HEAP8[78268] = 0;
   HEAP32[3049] = HEAP32[3049] + 1;
   $2 = down_trylock();
   HEAP32[3049] = HEAP32[3049] + -1;
   HEAP8[78268] = $1 & 1;
   if ($2) {
    break label$1
   }
   if (HEAPU8[68976]) {
    HEAP8[78268] = 0;
    HEAP32[3049] = HEAP32[3049] + 1;
    up();
    HEAP32[3049] = HEAP32[3049] + -1;
    HEAP8[78268] = $1 & 1;
    break label$1;
   }
   HEAP8[68980] = 1;
   HEAP8[68984] = 0;
   console_unlock();
  }
  if ($0 & 1) {
   __wake_up(11888, 1, 0)
  }
 }
 
 function __printk_safe_flush($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $1 = global$0 - 96 | 0;
  global$0 = $1;
  $9 = $0 + 12 | 0;
  $7 = $0 + -8 | 0;
  label$1 : {
   while (1) {
    $0 = $4;
    label$3 : {
     label$4 : {
      $4 = HEAP32[$7 >> 2];
      if ($4 >>> 0 > 236) {
       break label$4
      }
      if (($0 | 0) != 0 & $4 >>> 0 <= $0 >>> 0) {
       break label$4
      }
      if (!$4) {
       break label$1
      }
      if (($4 - $0 | 0) < 1) {
       break label$3
      }
      $6 = $4 + $9 | 0;
      $2 = $0 + $9 | 0;
      $3 = $2;
      while (1) {
       $8 = 1;
       $0 = $3;
       label$6 : {
        label$7 : {
         while (1) {
          label$8 : {
           label$10 : {
            label$11 : {
             $5 = HEAPU8[$0 | 0];
             if (($5 | 0) != 10) {
              $3 = $0 + 1 | 0;
              label$13 : {
               if ($3 >>> 0 >= $6 >>> 0) {
                break label$13
               }
               if (($5 | 0) != 1) {
                break label$13
               }
               $5 = HEAP8[$3 | 0];
               if (!$5) {
                break label$13
               }
               $10 = $5 + -48 >>> 0 < 8;
               if ($10) {
                break label$11
               }
               if ($5 + -99 >>> 0 <= 1) {
                break label$11
               }
              }
              $0 = $3;
              $8 = 0;
              break label$10;
             }
             HEAP32[$1 + 36 >> 2] = $2;
             HEAP32[$1 + 32 >> 2] = (1 - $2 | 0) + $0;
             printk_deferred(22115, $1 + 32 | 0);
             $3 = $0 + 1 | 0;
             $0 = $3;
             break label$7;
            }
            if (!$8) {
             break label$8
            }
            $8 = 1;
            if ($5 + -99 >>> 0 > 1 ? !$10 : 0) {
             break label$10
            }
            $0 = $0 + 2 | 0;
            if ($0 >>> 0 < $6 >>> 0) {
             continue
            }
            break label$6;
           }
           if ($0 >>> 0 < $6 >>> 0) {
            continue
           }
           break label$6;
          }
          break;
         };
         HEAP32[$1 + 84 >> 2] = $2;
         HEAP32[$1 + 80 >> 2] = $0 - $2;
         printk_deferred(22115, $1 + 80 | 0);
        }
        $2 = $0;
        if ($3 >>> 0 < $6 >>> 0) {
         continue
        }
        break label$3;
       }
       break;
      };
      if ($2 >>> 0 >= $6 >>> 0 | $8) {
       break label$3
      }
      HEAP32[$1 + 68 >> 2] = $2;
      HEAP32[$1 + 64 >> 2] = $6 - $2;
      printk_deferred(22115, $1 - -64 | 0);
      $0 = strlen(22120);
      HEAP32[$1 + 52 >> 2] = 22120;
      HEAP32[$1 + 48 >> 2] = $0;
      printk_deferred(22115, $1 + 48 | 0);
      break label$3;
     }
     $0 = strlen(22080);
     HEAP32[$1 + 20 >> 2] = 22080;
     HEAP32[$1 + 16 >> 2] = $0;
     printk_deferred(22115, $1 + 16 | 0);
     break label$1;
    }
    if (HEAP32[$7 >> 2] != ($4 | 0)) {
     continue
    }
    break;
   };
   HEAP32[$7 >> 2] = 0;
  }
  $0 = HEAP32[$7 + 4 >> 2];
  HEAP32[$7 + 4 >> 2] = 0;
  if ($0) {
   HEAP32[$1 >> 2] = $0;
   printk_deferred(22124, $1);
  }
  global$0 = $1 + 96 | 0;
 }
 
 function vprintk_func($0, $1) {
  var $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  label$1 : {
   label$3 : {
    label$4 : {
     if (!(HEAP32[3049] & 1073741824)) {
      if (HEAP32[3049] < 0) {
       break label$1
      }
      if (!(HEAP32[3049] & 1073741823)) {
       break label$4
      }
      HEAP32[$2 + 12 >> 2] = $1;
      while (1) {
       $1 = HEAP32[2985];
       if ($1 >>> 0 >= 235) {
        break label$3
       }
       label$7 : {
        if ($1) {
         break label$7
        }
       }
       $3 = HEAP32[$2 + 12 >> 2];
       HEAP32[$2 + 8 >> 2] = $3;
       $3 = vscnprintf($1 + 11960 | 0, 236 - $1 | 0, $0, $3);
       if (!$3) {
        break label$1
       }
       if (($1 | 0) != HEAP32[2985]) {
        continue
       }
       break;
      };
      HEAP32[2985] = $1 + $3;
      if (!HEAPU8[79296]) {
       break label$1
      }
      irq_work_queue(11948);
      break label$1;
     }
     vprintk_store($0, $1);
     HEAP32[2979] = HEAP32[2979] | 2;
     irq_work_queue(11920);
     break label$1;
    }
    vprintk_emit($0, $1);
    break label$1;
   }
   HEAP32[2986] = HEAP32[2986] + 1;
   if (!HEAPU8[79296]) {
    break label$1
   }
   irq_work_queue(11948);
  }
  global$0 = $2 + 16 | 0;
 }
 
 function __mutex_lock() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $3 = global$0 - 32 | 0;
  global$0 = $3;
  $1 = HEAP32[2];
  label$1 : {
   label$2 : {
    label$4 : {
     label$5 : {
      label$6 : {
       label$7 : {
        label$8 : {
         label$9 : {
          label$10 : {
           label$11 : {
            label$12 : {
             label$14 : {
              label$15 : {
               label$16 : {
                $2 = HEAP32[1125];
                $0 = $2 & -8;
                if ($0) {
                 if ($2 & 4 ? ($0 | 0) == ($1 | 0) : 0) {
                  break label$10
                 }
                 $2 = HEAP32[2];
                 $1 = HEAP32[1125];
                 $0 = $1 & -8;
                 if (!$0) {
                  break label$16
                 }
                 if ($1 & 4 ? ($0 | 0) == ($2 | 0) : 0) {
                  break label$9
                 }
                 HEAP32[$3 + 16 >> 2] = 4504;
                 $1 = HEAP32[1127];
                 HEAP32[$3 + 20 >> 2] = $1;
                 HEAP32[1127] = $3 + 16;
                 HEAP32[$1 >> 2] = $3 + 16;
                 if (HEAP32[1126] == ($3 + 16 | 0)) {
                  HEAP32[1125] = HEAP32[1125] | 1
                 }
                 HEAP32[$3 + 24 >> 2] = $2;
                 HEAP32[$3 + 12 >> 2] = 2;
                 HEAP32[HEAP32[2] >> 2] = 2;
                 $2 = HEAP32[2];
                 $0 = HEAP32[1125];
                 $1 = $0 & -8;
                 if (!$1) {
                  break label$15
                 }
                 while (1) {
                  if ($0 & 4 ? ($1 | 0) == ($2 | 0) : 0) {
                   break label$12
                  }
                  schedule();
                  label$24 : {
                   if (!($4 & 1)) {
                    $4 = 0;
                    if (HEAP32[1126] != ($3 + 16 | 0)) {
                     break label$24
                    }
                    HEAP32[1125] = HEAP32[1125] | 2;
                   }
                   $4 = 1;
                  }
                  HEAP32[$3 + 12 >> 2] = 2;
                  HEAP32[HEAP32[2] >> 2] = 2;
                  $2 = HEAP32[2];
                  $1 = HEAP32[1125];
                  $0 = $1 & -8;
                  if (!$0) {
                   break label$14
                  }
                  if ($1 & 4 ? ($0 | 0) == ($2 | 0) : 0) {
                   break label$11
                  }
                  $2 = HEAP32[2];
                  $0 = HEAP32[1125];
                  $1 = $0 & -8;
                  if ($1) {
                   continue
                  }
                  break;
                 };
                 $0 = $0 & 7;
                 break label$7;
                }
                $0 = $2 & 7;
                break label$2;
               }
               $0 = $1 & 7;
               break label$5;
              }
              $0 = $0 & 7;
              break label$7;
             }
             $0 = 7;
             break label$8;
            }
            $0 = $0 & 3;
            $2 = $1;
            break label$7;
           }
           $0 = 3;
           break label$8;
          }
          $0 = $2 & 3;
          break label$2;
         }
         $0 = $1 & 3;
         break label$5;
        }
        HEAP32[1125] = $2 | $0 & $1 & 5;
        break label$6;
       }
       HEAP32[1125] = $0 & 5 | $2;
      }
      HEAP32[HEAP32[2] >> 2] = 0;
      $2 = HEAP32[$3 + 20 >> 2];
      $1 = HEAP32[$3 + 16 >> 2];
      HEAP32[$2 >> 2] = $1;
      HEAP32[$1 + 4 >> 2] = $2;
      if (HEAP32[1126] != 4504) {
       break label$1
      }
      $4 = HEAP32[1125] & -8;
      break label$4;
     }
     $4 = $2 | $0 & 5;
    }
    HEAP32[1125] = $4;
    break label$1;
   }
   HEAP32[1125] = $1 | $0 & 5;
  }
  global$0 = $3 + 32 | 0;
 }
 
 function mutex_unlock($0) {
  if (HEAP32[$0 >> 2] == HEAP32[2]) {
   HEAP32[$0 >> 2] = 0;
   return;
  }
  __mutex_unlock_slowpath($0);
 }
 
 function __mutex_unlock_slowpath($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  HEAP32[$2 + 8 >> 2] = 1;
  $1 = HEAP32[$0 >> 2];
  HEAP32[$2 + 12 >> 2] = $2 + 8;
  $3 = $1 & 2;
  label$1 : {
   if (!$3) {
    HEAP32[$0 >> 2] = $1 & 7;
    if (!($1 & 1)) {
     break label$1
    }
   }
   $1 = HEAP32[$0 + 4 >> 2];
   label$4 : {
    if (($1 | 0) != ($0 + 4 | 0)) {
     $1 = HEAP32[$1 + 8 >> 2];
     wake_q_add($2 + 8 | 0, $1);
     break label$4;
    }
    $1 = 0;
   }
   if ($3) {
    $3 = $0;
    $0 = HEAP32[$0 >> 2] & 1 | $1;
    HEAP32[$3 >> 2] = $1 ? $0 | 4 : $0;
   }
   wake_up_q($2 + 8 | 0);
  }
  global$0 = $2 + 16 | 0;
 }
 
 function mutex_trylock($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $3 = $0;
  $1 = HEAP32[2];
  $4 = $1;
  label$1 : {
   label$2 : {
    $0 = HEAP32[$0 >> 2];
    $2 = $0 & -8;
    if ($2) {
     if ($0 & 4 ? ($1 | 0) == ($2 | 0) : 0) {
      break label$2
     }
     return 0;
    }
    $0 = $0 & 7;
    break label$1;
   }
   $0 = $0 & 3;
  }
  HEAP32[$3 >> 2] = $4 | $0 & 5;
  return 1;
 }
 
 function down() {
  var $0 = 0, $1 = 0;
  $1 = HEAPU8[79300];
  HEAP8[79300] = 0;
  $0 = HEAP32[2976];
  label$1 : {
   if ($0) {
    HEAP32[2976] = $0 + -1;
    break label$1;
   }
   __down();
  }
  HEAP8[79300] = $1 & 1;
 }
 
 function __down() {
  var $0 = 0, $1 = 0, $2 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 11908;
  $1 = HEAP32[2978];
  HEAP32[$0 + 4 >> 2] = $1;
  HEAP32[2978] = $0;
  HEAP32[$1 >> 2] = $0;
  HEAP8[$0 + 12 | 0] = 0;
  HEAP32[$0 + 8 >> 2] = HEAP32[2];
  $1 = 2147483647;
  $2 = $0 + 12 | 0;
  label$1 : {
   while (1) {
    if (($1 | 0) >= 1) {
     HEAP32[HEAP32[2] >> 2] = 2;
     HEAP8[79300] = 1;
     $1 = schedule_timeout($1);
     HEAP8[79300] = 0;
     if (!HEAPU8[$2 | 0]) {
      continue
     }
     break label$1;
    }
    break;
   };
   $1 = HEAP32[$0 >> 2];
   $2 = HEAP32[$0 + 4 >> 2];
   HEAP32[$1 + 4 >> 2] = $2;
   HEAP32[$2 >> 2] = $1;
   HEAP32[$0 >> 2] = 256;
   HEAP32[$0 + 4 >> 2] = 512;
  }
  global$0 = $0 + 16 | 0;
 }
 
 function down_trylock() {
  var $0 = 0, $1 = 0;
  $1 = HEAPU8[79300];
  HEAP8[79300] = 0;
  $0 = HEAP32[2976] + -1 | 0;
  if (($0 | 0) >= 0) {
   HEAP32[2976] = $0
  }
  HEAP8[79300] = $1 & 1;
  return $0 >>> 31 | 0;
 }
 
 function up() {
  var $0 = 0;
  $0 = HEAPU8[79300];
  HEAP8[79300] = 0;
  label$1 : {
   if (HEAP32[2977] == 11908) {
    HEAP32[2976] = HEAP32[2976] + 1;
    break label$1;
   }
   __up();
  }
  HEAP8[79300] = $0 & 1;
 }
 
 function __up() {
  var $0 = 0, $1 = 0, $2 = 0;
  $0 = HEAP32[2977];
  $1 = HEAP32[$0 >> 2];
  $2 = HEAP32[$0 + 4 >> 2];
  HEAP32[$1 + 4 >> 2] = $2;
  HEAP8[$0 + 12 | 0] = 1;
  HEAP32[$2 >> 2] = $1;
  HEAP32[$0 >> 2] = 256;
  HEAP32[$0 + 4 >> 2] = 512;
  wake_up_process(HEAP32[$0 + 8 >> 2]);
 }
 
 function __down_read($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 32 | 0;
  global$0 = $1;
  $2 = HEAP32[$0 >> 2];
  label$1 : {
   if (!(HEAP32[$0 + 4 >> 2] == ($0 + 4 | 0) ? ($2 | 0) >= 0 : 0)) {
    HEAP32[$1 + 28 >> 2] = 1;
    $2 = HEAP32[2];
    HEAP32[$2 + 8 >> 2] = HEAP32[$2 + 8 >> 2] + 1;
    HEAP32[$1 + 24 >> 2] = $2;
    HEAP32[$1 + 16 >> 2] = $0 + 4;
    $3 = $0 + 8 | 0;
    $0 = HEAP32[$3 >> 2];
    HEAP32[$3 >> 2] = $1 + 16;
    HEAP32[$1 + 20 >> 2] = $0;
    HEAP32[$0 >> 2] = $1 + 16;
    if ($2) {
     $0 = $1 + 24 | 0;
     while (1) {
      HEAP32[$1 + 12 >> 2] = 2;
      HEAP32[HEAP32[2] >> 2] = 2;
      schedule();
      if (HEAP32[$0 >> 2]) {
       continue
      }
      break;
     };
    }
    break label$1;
   }
   HEAP32[$0 >> 2] = $2 + 1;
  }
  global$0 = $1 + 32 | 0;
 }
 
 function __down_read_trylock($0) {
  var $1 = 0;
  $1 = HEAP32[$0 >> 2];
  if (!(HEAP32[$0 + 4 >> 2] == ($0 + 4 | 0) ? ($1 | 0) >= 0 : 0)) {
   return 0
  }
  HEAP32[$0 >> 2] = $1 + 1;
  return 1;
 }
 
 function __down_write($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $1 = global$0 - 32 | 0;
  global$0 = $1;
  $4 = $0 + 8 | 0;
  $2 = HEAP32[$4 >> 2];
  HEAP32[$2 >> 2] = $1 + 16;
  $3 = $0 + 4 | 0;
  HEAP32[$1 + 16 >> 2] = $3;
  HEAP32[$4 >> 2] = $1 + 16;
  HEAP32[$1 + 20 >> 2] = $2;
  HEAP32[$1 + 28 >> 2] = 0;
  HEAP32[$1 + 24 >> 2] = HEAP32[2];
  if (HEAP32[$0 >> 2]) {
   while (1) {
    HEAP32[$1 + 12 >> 2] = 2;
    HEAP32[HEAP32[2] >> 2] = 2;
    schedule();
    if (HEAP32[$0 >> 2]) {
     continue
    }
    break;
   };
   $3 = HEAP32[$1 + 16 >> 2];
   $2 = HEAP32[$1 + 20 >> 2];
  }
  HEAP32[$0 >> 2] = -1;
  HEAP32[$3 + 4 >> 2] = $2;
  HEAP32[$2 >> 2] = $3;
  HEAP32[$1 + 16 >> 2] = 256;
  HEAP32[$1 + 20 >> 2] = 512;
  global$0 = $1 + 32 | 0;
 }
 
 function __up_read($0) {
  var $1 = 0;
  $1 = HEAP32[$0 >> 2] + -1 | 0;
  HEAP32[$0 >> 2] = $1;
  label$1 : {
   if ($1) {
    break label$1
   }
   $1 = $0 + 4 | 0;
   $0 = HEAP32[$0 + 4 >> 2];
   if (($1 | 0) == ($0 | 0)) {
    break label$1
   }
   wake_up_process(HEAP32[$0 + 8 >> 2]);
  }
 }
 
 function __up_write($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  HEAP32[$0 >> 2] = 0;
  label$1 : {
   $4 = $0 + 4 | 0;
   $1 = HEAP32[$0 + 4 >> 2];
   if (($4 | 0) != ($1 | 0)) {
    if (!HEAP32[$1 + 12 >> 2]) {
     break label$1
    }
    $2 = 1;
    while (1) {
     $5 = $2;
     $3 = HEAP32[$1 >> 2];
     $2 = HEAP32[$1 + 4 >> 2];
     HEAP32[$3 + 4 >> 2] = $2;
     HEAP32[$2 >> 2] = $3;
     HEAP32[$1 >> 2] = 256;
     HEAP32[$1 + 4 >> 2] = 512;
     $2 = HEAP32[$1 + 8 >> 2];
     HEAP32[$1 + 8 >> 2] = 0;
     wake_up_process($2);
     $1 = HEAP32[$2 + 8 >> 2] + -1 | 0;
     HEAP32[$2 + 8 >> 2] = $1;
     if (!$1) {
      __put_task_struct($2)
     }
     if (($3 | 0) != ($4 | 0)) {
      $2 = $5 + 1 | 0;
      $1 = $3;
      if (HEAP32[$1 + 12 >> 2]) {
       continue
      }
     }
     break;
    };
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + $5;
   }
   return;
  }
  wake_up_process(HEAP32[$1 + 8 >> 2]);
 }
 
 function task_rq_lock($0, $1) {
  var $2 = 0;
  $2 = HEAP32[19828];
  HEAP32[19828] = 0;
  HEAP32[$1 >> 2] = $2;
  label$1 : {
   if (HEAP32[$0 + 20 >> 2] != 2) {
    break label$1
   }
   $0 = $0 + 20 | 0;
   while (1) {
    label$2 : {
     HEAP32[19828] = HEAP32[$1 >> 2];
     if (HEAP32[$0 >> 2] == 2) {
      break label$2
     }
     $2 = HEAP32[19828];
     HEAP32[19828] = 0;
     HEAP32[$1 >> 2] = $2;
     if (HEAP32[$0 >> 2] == 2) {
      continue
     }
     break label$1;
    }
    break;
   };
   while (1) continue;
  }
  return 12208;
 }
 
 function update_rq_clock($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  label$1 : {
   if (HEAPU8[$0 + 1056 | 0] & 2) {
    break label$1
   }
   $1 = sched_clock_cpu();
   $5 = i64toi32_i32$HIGH_BITS;
   $2 = HEAP32[$0 + 1064 >> 2];
   $3 = $5 - (HEAP32[$0 + 1068 >> 2] + ($1 >>> 0 < $2 >>> 0) | 0) | 0;
   $2 = $1 - $2 | 0;
   if (($3 | 0) < 0) {
    $4 = 1
   } else {
    if (($3 | 0) <= 0) {
     $4 = $2 >>> 0 >= 0 ? 0 : 1
    } else {
     $4 = 0
    }
   }
   if ($4) {
    break label$1
   }
   $4 = $0 + 1064 | 0;
   HEAP32[$4 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = $5;
   $1 = $3 + HEAP32[$0 + 1076 >> 2] | 0;
   $3 = $2 + HEAP32[$0 + 1072 >> 2] | 0;
   if ($3 >>> 0 < $2 >>> 0) {
    $1 = $1 + 1 | 0
   }
   HEAP32[$0 + 1072 >> 2] = $3;
   HEAP32[$0 + 1076 >> 2] = $1;
  }
 }
 
 function wake_q_add($0, $1) {
  var $2 = 0, $3 = 0;
  $3 = HEAP32[19828];
  HEAP32[19828] = 0;
  if (HEAP32[$1 + 792 >> 2]) {
   HEAP32[19828] = $3;
   return;
  }
  $2 = $1 + 792 | 0;
  HEAP32[HEAP32[$0 + 4 >> 2] >> 2] = $2;
  HEAP32[19828] = $3;
  HEAP32[$2 >> 2] = 1;
  HEAP32[$1 + 8 >> 2] = HEAP32[$1 + 8 >> 2] + 1;
  HEAP32[$0 + 4 >> 2] = $2;
 }
 
 function wake_up_q($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  label$1 : {
   $0 = HEAP32[$0 >> 2];
   if (($0 | 0) != 1) {
    while (1) {
     $2 = $0 + -792 | 0;
     if (!$2) {
      break label$1
     }
     $4 = HEAP32[$0 >> 2];
     HEAP32[$0 >> 2] = 0;
     try_to_wake_up($2, 3, 0);
     $3 = $0 + -784 | 0;
     $0 = HEAP32[$3 >> 2] + -1 | 0;
     HEAP32[$3 >> 2] = $0;
     if (!$0) {
      __put_task_struct($2)
     }
     $0 = $4;
     if (($0 | 0) != 1) {
      continue
     }
     break;
    }
   }
   global$0 = $1 + 16 | 0;
   return;
  }
  HEAP32[$1 + 8 >> 2] = 22183;
  HEAP32[$1 + 4 >> 2] = 431;
  HEAP32[$1 >> 2] = 22176;
  printk(22145, $1);
  abort();
 }
 
 function try_to_wake_up($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $7 = HEAP32[19828];
  HEAP32[19828] = 0;
  label$1 : {
   if (!(HEAP32[$0 >> 2] & $1)) {
    break label$1
   }
   label$2 : {
    label$3 : {
     label$4 : {
      if (HEAP32[$0 + 20 >> 2]) {
       label$6 : {
        $1 = $0 + 20 | 0;
        $3 = HEAP32[$1 >> 2];
        if (($3 | 0) != 2) {
         break label$6
        }
        while (1) {
         if (HEAP32[$1 >> 2] != 2) {
          $3 = HEAP32[$1 >> 2];
          if (($3 | 0) == 2) {
           continue
          }
          break label$6;
         }
         break;
        };
        while (1) continue;
       }
       if (($3 | 0) == 1) {
        label$11 : {
         if (HEAPU8[13264] & 2) {
          break label$11
         }
         $1 = sched_clock_cpu();
         $5 = i64toi32_i32$HIGH_BITS;
         $4 = HEAP32[3318];
         $3 = $5 - (HEAP32[3319] + ($1 >>> 0 < $4 >>> 0) | 0) | 0;
         $4 = $1 - $4 | 0;
         if (($3 | 0) < 0) {
          $6 = 1
         } else {
          if (($3 | 0) <= 0) {
           $6 = $4 >>> 0 >= 0 ? 0 : 1
          } else {
           $6 = 0
          }
         }
         if ($6) {
          break label$11
         }
         HEAP32[3318] = $1;
         HEAP32[3319] = $5;
         $1 = $3 + HEAP32[3321] | 0;
         $3 = $4 + HEAP32[3320] | 0;
         if ($3 >>> 0 < $4 >>> 0) {
          $1 = $1 + 1 | 0
         }
         HEAP32[3320] = $3;
         HEAP32[3321] = $1;
        }
        $4 = HEAP32[$0 + 40 >> 2];
        $1 = HEAP32[3311];
        $5 = HEAP32[$1 + 40 >> 2];
        if (($4 | 0) == ($5 | 0)) {
         break label$4
        }
        $3 = 23104;
        if (($5 | 0) == 23104) {
         break label$2
        }
        while (1) {
         if (($3 | 0) == ($4 | 0)) {
          break label$3
         }
         $3 = HEAP32[$3 >> 2];
         if (!$3) {
          break label$2
         }
         if (($3 | 0) != ($5 | 0)) {
          continue
         }
         break;
        };
        break label$2;
       }
      }
      if (HEAPU8[$0 + 440 | 0] & 2) {
       HEAP32[3322] = HEAP32[3322] + -1
      }
      label$14 : {
       if (HEAPU8[13264] & 2) {
        break label$14
       }
       $1 = sched_clock_cpu();
       $5 = i64toi32_i32$HIGH_BITS;
       $4 = HEAP32[3318];
       $3 = $5 - (HEAP32[3319] + ($1 >>> 0 < $4 >>> 0) | 0) | 0;
       $4 = $1 - $4 | 0;
       if (($3 | 0) < 0) {
        $6 = 1
       } else {
        if (($3 | 0) <= 0) {
         $6 = $4 >>> 0 >= 0 ? 0 : 1
        } else {
         $6 = 0
        }
       }
       if ($6) {
        break label$14
       }
       HEAP32[3318] = $1;
       HEAP32[3319] = $5;
       $1 = $3 + HEAP32[3321] | 0;
       $3 = $4 + HEAP32[3320] | 0;
       if ($3 >>> 0 < $4 >>> 0) {
        $1 = $1 + 1 | 0
       }
       HEAP32[3320] = $3;
       HEAP32[3321] = $1;
      }
      label$15 : {
       if (!(HEAP32[$0 >> 2] & 2)) {
        break label$15
       }
       if (HEAP8[$0 + 14 | 0] & 1) {
        break label$15
       }
       if (HEAP32[$0 >> 2] & 1024) {
        break label$15
       }
       HEAP32[3310] = HEAP32[3310] + -1;
      }
      FUNCTION_TABLE[HEAP32[HEAP32[$0 + 40 >> 2] + 4 >> 2]](12208, $0, 9);
      HEAP32[$0 + 20 >> 2] = 1;
      if (HEAPU8[$0 + 12 | 0] & 32) {
       $1 = HEAP32[HEAP32[$0 + 604 >> 2] + 8 >> 2];
       if (!(HEAPU16[$1 + 48 >> 1] & 456)) {
        $1 = HEAP32[$1 + 32 >> 2];
        HEAP32[$1 + 380 >> 2] = HEAP32[$1 + 380 >> 2] + 1;
       }
      }
      label$17 : {
       label$18 : {
        $4 = HEAP32[$0 + 40 >> 2];
        $1 = HEAP32[3311];
        $5 = HEAP32[$1 + 40 >> 2];
        if (($4 | 0) != ($5 | 0)) {
         $3 = 23104;
         if (($5 | 0) == 23104) {
          break label$17
         }
         while (1) {
          if (($3 | 0) == ($4 | 0)) {
           break label$18
          }
          $3 = HEAP32[$3 >> 2];
          if (!$3) {
           break label$17
          }
          if (($3 | 0) != ($5 | 0)) {
           continue
          }
          break;
         };
         break label$17;
        }
        FUNCTION_TABLE[HEAP32[$4 + 20 >> 2]](12208, $0, $2);
        $1 = HEAP32[3311];
        break label$17;
       }
       $2 = HEAP32[$1 + 4 >> 2];
       $3 = HEAP32[$2 >> 2];
       if ($3 & 8) {
        break label$17
       }
       HEAP32[$2 >> 2] = $3 | 8;
      }
      $3 = 1;
      label$21 : {
       if (HEAP32[$1 + 20 >> 2] != 1) {
        break label$21
       }
       if (!(HEAP32[HEAP32[$1 + 4 >> 2] >> 2] & 8)) {
        break label$21
       }
       HEAP32[3316] = HEAP32[3316] | 1;
      }
      HEAP32[$0 >> 2] = 0;
      break label$1;
     }
     FUNCTION_TABLE[HEAP32[$4 + 20 >> 2]](12208, $0, $2);
     $1 = HEAP32[3311];
     break label$2;
    }
    $2 = HEAP32[$1 + 4 >> 2];
    $3 = HEAP32[$2 >> 2];
    if ($3 & 8) {
     break label$2
    }
    HEAP32[$2 >> 2] = $3 | 8;
   }
   $3 = 1;
   label$22 : {
    if (HEAP32[$1 + 20 >> 2] != 1) {
     break label$22
    }
    if (!(HEAP32[HEAP32[$1 + 4 >> 2] >> 2] & 8)) {
     break label$22
    }
    HEAP32[3316] = HEAP32[3316] | 1;
   }
   HEAP32[$0 >> 2] = 0;
  }
  HEAP32[19828] = $7;
  return $3;
 }
 
 function wake_up_process($0) {
  try_to_wake_up($0, 3, 0);
 }
 
 function resched_cpu() {
  var $0 = 0, $1 = 0, $2 = 0;
  $2 = HEAP32[19828];
  HEAP32[19828] = 0;
  $0 = HEAP32[HEAP32[3311] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
  HEAP32[19828] = $2;
 }
 
 function check_preempt_curr($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  label$1 : {
   label$2 : {
    $3 = HEAP32[$0 + 1036 >> 2];
    $2 = HEAP32[$3 + 40 >> 2];
    $4 = HEAP32[$1 + 40 >> 2];
    if (($2 | 0) != ($4 | 0)) {
     $1 = 23104;
     if (($2 | 0) == 23104) {
      break label$1
     }
     while (1) {
      if (($1 | 0) == ($4 | 0)) {
       break label$2
      }
      $1 = HEAP32[$1 >> 2];
      if (!$1) {
       break label$1
      }
      if (($1 | 0) != ($2 | 0)) {
       continue
      }
      break;
     };
     break label$1;
    }
    FUNCTION_TABLE[HEAP32[$4 + 20 >> 2]]($0, $1, 0);
    $3 = HEAP32[$0 + 1036 >> 2];
    break label$1;
   }
   $1 = HEAP32[$3 + 4 >> 2];
   $2 = HEAP32[$1 >> 2];
   if ($2 & 8) {
    break label$1
   }
   HEAP32[$1 >> 2] = $2 | 8;
  }
  label$5 : {
   if (HEAP32[$3 + 20 >> 2] != 1) {
    break label$5
   }
   if (!(HEAP32[HEAP32[$3 + 4 >> 2] >> 2] & 8)) {
    break label$5
   }
   HEAP32[$0 + 1056 >> 2] = HEAP32[$0 + 1056 >> 2] | 1;
  }
 }
 
 function wake_up_state($0, $1) {
  try_to_wake_up($0, $1, 0);
 }
 
 function to_ratio($0, $1, $2, $3) {
  label$1 : {
   if (!(($2 | 0) == -1 & ($3 | 0) == -1)) {
    if (!($0 | $1)) {
     break label$1
    }
    $3 = $3 << 20 | $2 >>> 12;
    return div64_u64($2 << 20, $3, $0, $1);
   }
   return 1048576;
  }
  return 0;
 }
 
 function __schedule($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  $3 = HEAP32[3311];
  if (HEAP32[1]) {
   __schedule_bug($3);
   HEAP32[1] = 0;
  }
  HEAP32[19828] = 0;
  $1 = HEAP32[2966];
  if (($1 | 0) != HEAP32[2965]) {
   HEAP32[2965] = $1;
   raise_softirq();
  }
  $1 = HEAP32[3316];
  HEAP32[3316] = $1 << 1;
  label$2 : {
   if ($1 & 1) {
    break label$2
   }
   $1 = sched_clock_cpu();
   $6 = i64toi32_i32$HIGH_BITS;
   $4 = HEAP32[3318];
   $2 = $6 - (HEAP32[3319] + ($1 >>> 0 < $4 >>> 0) | 0) | 0;
   $4 = $1 - $4 | 0;
   if (($2 | 0) < 0) {
    $7 = 1
   } else {
    if (($2 | 0) <= 0) {
     $7 = $4 >>> 0 >= 0 ? 0 : 1
    } else {
     $7 = 0
    }
   }
   if ($7) {
    break label$2
   }
   HEAP32[3318] = $1;
   HEAP32[3319] = $6;
   $1 = $2 + HEAP32[3321] | 0;
   $2 = $4 + HEAP32[3320] | 0;
   if ($2 >>> 0 < $4 >>> 0) {
    $1 = $1 + 1 | 0
   }
   HEAP32[3320] = $2;
   HEAP32[3321] = $1;
  }
  $1 = $3 + 660 | 0;
  label$3 : {
   if ($0) {
    break label$3
   }
   if (!HEAP32[$3 >> 2]) {
    break label$3
   }
   label$4 : {
    label$6 : {
     label$7 : {
      label$8 : {
       label$9 : {
        $0 = HEAP32[$3 >> 2];
        label$10 : {
         if (!($0 & 257)) {
          break label$10
         }
         if (!(HEAP32[HEAP32[$3 + 4 >> 2] >> 2] & 4)) {
          break label$10
         }
         if ($0 & 1) {
          break label$9
         }
         if (HEAP8[$3 + 761 | 0] & 1) {
          break label$9
         }
        }
        label$11 : {
         if (!(HEAP32[$3 >> 2] & 2)) {
          break label$11
         }
         if (HEAP8[$3 + 14 | 0] & 1) {
          break label$11
         }
         if (HEAP32[$3 >> 2] & 1024) {
          break label$11
         }
         HEAP32[3310] = HEAP32[3310] + 1;
        }
        FUNCTION_TABLE[HEAP32[HEAP32[$3 + 40 >> 2] + 8 >> 2]](12208, $3, 9);
        HEAP32[$3 + 20 >> 2] = 0;
        if (HEAPU8[$3 + 440 | 0] & 2) {
         HEAP32[3322] = HEAP32[3322] + 1
        }
        if (!(HEAPU8[$3 + 12 | 0] & 32)) {
         break label$4
        }
        $1 = wq_worker_sleeping($3);
        if (!$1) {
         break label$4
        }
        if (($1 | 0) == HEAP32[2]) {
         break label$4
        }
        if (!(HEAP32[$1 >> 2] & 3)) {
         break label$4
        }
        label$13 : {
         if (HEAP32[$1 + 20 >> 2] == 1) {
          break label$13
         }
         if (HEAPU8[$1 + 440 | 0] & 2) {
          HEAP32[3322] = HEAP32[3322] + -1
         }
         label$15 : {
          if (!(HEAP32[$1 >> 2] & 2)) {
           break label$15
          }
          if (HEAP8[$1 + 14 | 0] & 1) {
           break label$15
          }
          if (HEAP32[$1 >> 2] & 1024) {
           break label$15
          }
          HEAP32[3310] = HEAP32[3310] + -1;
         }
         FUNCTION_TABLE[HEAP32[HEAP32[$1 + 40 >> 2] + 4 >> 2]](12208, $1, 9);
         HEAP32[$1 + 20 >> 2] = 1;
         if (!(HEAPU8[$1 + 12 | 0] & 32)) {
          break label$13
         }
         $0 = HEAP32[HEAP32[$1 + 604 >> 2] + 8 >> 2];
         if (!(HEAPU16[$0 + 48 >> 1] & 456)) {
          $0 = HEAP32[$0 + 32 >> 2];
          HEAP32[$0 + 380 >> 2] = HEAP32[$0 + 380 >> 2] + 1;
         }
        }
        $4 = HEAP32[$1 + 40 >> 2];
        $2 = HEAP32[3311];
        $6 = HEAP32[$2 + 40 >> 2];
        if (($4 | 0) == ($6 | 0)) {
         break label$8
        }
        $0 = 23104;
        if (($6 | 0) == 23104) {
         break label$6
        }
        while (1) {
         if (($0 | 0) == ($4 | 0)) {
          break label$7
         }
         $0 = HEAP32[$0 >> 2];
         if (!$0) {
          break label$6
         }
         if (($0 | 0) != ($6 | 0)) {
          continue
         }
         break;
        };
        break label$6;
       }
       HEAP32[$3 >> 2] = 0;
       break label$4;
      }
      FUNCTION_TABLE[HEAP32[$4 + 20 >> 2]](12208, $1, 0);
      $2 = HEAP32[3311];
      break label$6;
     }
     $0 = HEAP32[$2 + 4 >> 2];
     $4 = HEAP32[$0 >> 2];
     if ($4 & 8) {
      break label$6
     }
     HEAP32[$0 >> 2] = $4 | 8;
    }
    label$17 : {
     if (HEAP32[$2 + 20 >> 2] != 1) {
      break label$17
     }
     if (!(HEAP32[HEAP32[$2 + 4 >> 2] >> 2] & 8)) {
      break label$17
     }
     HEAP32[3316] = HEAP32[3316] | 1;
    }
    HEAP32[$1 >> 2] = 0;
   }
   $1 = $3 + 656 | 0;
  }
  label$18 : {
   label$19 : {
    label$20 : {
     label$21 : {
      $0 = HEAP32[$3 + 40 >> 2];
      if (($0 | 0) != 22800 ? ($0 | 0) != 22616 : 0) {
       break label$21
      }
      if (HEAP32[3052] != HEAP32[3068]) {
       break label$21
      }
      $0 = FUNCTION_TABLE[HEAP32[5706]](12208, $3, $5 + 24 | 0) | 0;
      if (($0 | 0) == -1) {
       break label$21
      }
      if (!$0) {
       break label$20
      }
      break label$18;
     }
     $2 = 23104;
     while (1) {
      $0 = FUNCTION_TABLE[HEAP32[$2 + 24 >> 2]](12208, $3, $5 + 24 | 0) | 0;
      if (!$0) {
       $2 = HEAP32[$2 >> 2];
       if ($2) {
        continue
       }
       break label$19;
      }
      $2 = 23104;
      if (($0 | 0) == -1) {
       continue
      }
      break;
     };
     break label$18;
    }
    $0 = FUNCTION_TABLE[HEAP32[5660]](12208, $3, $5 + 24 | 0) | 0;
    break label$18;
   }
   HEAP32[$5 + 8 >> 2] = 22599;
   HEAP32[$5 + 4 >> 2] = 3340;
   HEAP32[$5 >> 2] = 22176;
   printk(22145, $5);
   abort();
  }
  $2 = HEAP32[$3 + 4 >> 2];
  HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -9;
  label$25 : {
   label$27 : {
    label$28 : {
     label$29 : {
      label$30 : {
       if (($0 | 0) != ($3 | 0)) {
        HEAP32[3311] = $0;
        $2 = HEAP32[3063];
        $4 = HEAP32[3062] + 1 | 0;
        if ($4 >>> 0 < 1) {
         $2 = $2 + 1 | 0
        }
        HEAP32[3062] = $4;
        HEAP32[3063] = $2;
        HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
        $1 = HEAP32[$3 + 360 >> 2];
        $2 = HEAP32[$0 + 356 >> 2];
        if ($2) {
         HEAP32[$2 + 400 >> 2] = HEAP32[$2 + 400 >> 2] | 1;
         $4 = HEAP32[19828];
         HEAP32[19828] = 0;
         $0 = HEAP32[4942];
         if (($0 ^ HEAP32[$2 + 360 >> 2]) >>> 0 < 256) {
          break label$28
         }
         $0 = $0 + 1 | 0;
         HEAP32[4942] = $0;
         if (!($0 & 255)) {
          break label$30
         }
         break label$29;
        }
        HEAP32[$0 + 360 >> 2] = $1;
        HEAP32[$1 + 44 >> 2] = HEAP32[$1 + 44 >> 2] + 1;
        break label$27;
       }
       HEAP32[19828] = 1;
       HEAP32[3316] = HEAP32[3316] & -4;
       break label$25;
      }
      utlb_invalidate();
      abort();
     }
     HEAP32[$2 + 360 >> 2] = $0;
    }
    HEAP32[19828] = $4;
   }
   if (!HEAP32[$3 + 356 >> 2]) {
    HEAP32[3315] = $1;
    HEAP32[$3 + 360 >> 2] = 0;
   }
   HEAP32[3316] = HEAP32[3316] & -4;
   printk(31182, 0);
   $0 = HEAP32[3315];
   if (HEAP32[1]) {
    HEAP32[1] = 0
   }
   $3 = HEAP32[0];
   HEAP32[19828] = 1;
   HEAP32[3315] = 0;
   label$34 : {
    if (!$0) {
     break label$34
    }
    $1 = HEAP32[$0 + 44 >> 2] + -1 | 0;
    HEAP32[$0 + 44 >> 2] = $1;
    if ($1) {
     break label$34
    }
    __mmdrop($0);
   }
   if (($3 | 0) != 128) {
    break label$25
   }
   $0 = HEAP32[HEAP32[10] + 44 >> 2];
   if ($0) {
    FUNCTION_TABLE[$0](0)
   }
   $0 = HEAP32[2] + -1 | 0;
   HEAP32[2] = $0;
   if ($0) {
    break label$25
   }
   __put_task_struct(0);
  }
  global$0 = $5 + 32 | 0;
 }
 
 function __schedule_bug($0) {
  var $1 = 0, $2 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  label$1 : {
   if (HEAP32[19560]) {
    break label$1
   }
   $2 = HEAP32[$0 + 496 >> 2];
   HEAP32[$1 + 8 >> 2] = HEAP32[1];
   HEAP32[$1 + 4 >> 2] = $2;
   HEAP32[$1 >> 2] = $0 + 700;
   printk(22528, $1);
   if (!HEAP32[16692]) {
    dump_stack();
    add_taint(9, 0);
    break label$1;
   }
   panic(22574, 0);
   abort();
  }
  global$0 = $1 + 16 | 0;
 }
 
 function schedule() {
  while (1) {
   __schedule(0);
   if (HEAP32[0] & 8) {
    continue
   }
   break;
  };
 }
 
 function default_wake_function($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  return try_to_wake_up(HEAP32[$0 + 4 >> 2], $1, $2) | 0;
 }
 
 function _cond_resched() {
  label$1 : {
   if (HEAP32[1]) {
    break label$1
   }
   if (!(HEAP32[0] & 8)) {
    break label$1
   }
   while (1) {
    __schedule(1);
    if (HEAP32[0] & 8) {
     continue
    }
    break;
   };
  }
 }
 
 function io_schedule_timeout() {
  var $0 = 0, $1 = 0;
  $0 = HEAP32[2];
  $1 = HEAPU8[$0 + 440 | 0];
  HEAP8[$0 + 440 | 0] = $1 | 2;
  schedule_timeout(25);
  $0 = HEAP32[2];
  HEAP8[$0 + 440 | 0] = HEAPU8[$0 + 440 | 0] & 253 | $1 & 2;
 }
 
 function io_schedule() {
  var $0 = 0, $1 = 0;
  $0 = HEAP32[2];
  $1 = HEAPU8[$0 + 440 | 0];
  HEAP8[$0 + 440 | 0] = $1 | 2;
  while (1) {
   __schedule(0);
   if (HEAP32[0] & 8) {
    continue
   }
   break;
  };
  $0 = HEAP32[2];
  HEAP8[$0 + 440 | 0] = HEAPU8[$0 + 440 | 0] & 253 | $1 & 2;
 }
 
 function init_idle($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $4 = HEAP32[19828];
  HEAP32[19828] = 0;
  $1 = $0 + 120 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 112 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 96 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $2 = $0 + 88 | 0;
  $1 = $2;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  HEAP32[$0 + 80 >> 2] = 0;
  HEAP32[$0 + 20 >> 2] = 0;
  $1 = $0 + 104 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 160 | 0;
  HEAP32[$0 + 160 >> 2] = $1;
  $3 = $0 + 72 | 0;
  HEAP32[$0 + 76 >> 2] = $3;
  HEAP32[$3 >> 2] = $3;
  hrtimer_init($1 + 80 | 0);
  HEAP32[$1 + 112 >> 2] = 61;
  hrtimer_init($1 + 128 | 0);
  HEAP32[$1 + 160 >> 2] = 62;
  __dl_clear_params($0);
  $1 = $0 + 128 | 0;
  HEAP32[$0 + 128 >> 2] = $1;
  HEAP32[$0 + 136 >> 2] = 0;
  HEAP32[$0 + 132 >> 2] = $1;
  HEAP32[$0 + 148 >> 2] = 0;
  HEAP32[$0 + 144 >> 2] = HEAP32[3361];
  HEAP32[$0 >> 2] = 0;
  (wasm2js_i32$0 = $2, wasm2js_i32$1 = sched_clock()), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  HEAP32[$2 + 4 >> 2] = i64toi32_i32$HIGH_BITS;
  HEAP32[3312] = $0;
  HEAP32[3311] = $0;
  HEAP32[$0 + 20 >> 2] = 1;
  HEAP32[$0 + 12 >> 2] = HEAP32[$0 + 12 >> 2] | 2;
  HEAP32[19828] = $4;
  HEAP32[$0 + 40 >> 2] = 22616;
  HEAP32[HEAP32[$0 + 4 >> 2] + 4 >> 2] = 0;
 }
 
 function sched_init() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  while (1) {
   $1 = $0 + 13568 | 0;
   HEAP32[$1 + 4 >> 2] = $1;
   HEAP32[$1 >> 2] = $1;
   $0 = $0 + 8 | 0;
   if (($0 | 0) != 2048) {
    continue
   }
   break;
  };
  $2 = __wasm_i64_mul(HEAP32[3050], 0, 1e3, 0);
  $3 = i64toi32_i32$HIGH_BITS;
  $1 = HEAP32[3051];
  $0 = $1 >> 31;
  $4 = __wasm_i64_mul($1, $0, 1e3, 0);
  $5 = i64toi32_i32$HIGH_BITS;
  if (($0 | 0) < 0) {
   $0 = 1
  } else {
   if (($0 | 0) <= 0) {
    $0 = $1 >>> 0 >= 0 ? 0 : 1
   } else {
    $0 = 0
   }
  }
  init_rt_bandwidth($2, $3, $0 ? -1 : $4, $0 ? -1 : $5);
  $2 = __wasm_i64_mul(HEAP32[3050], 0, 1e3, 0);
  $3 = i64toi32_i32$HIGH_BITS;
  $1 = HEAP32[3051];
  $0 = $1 >> 31;
  $4 = __wasm_i64_mul($1, $0, 1e3, 0);
  $5 = i64toi32_i32$HIGH_BITS;
  if (($0 | 0) < 0) {
   $0 = 1
  } else {
   if (($0 | 0) <= 0) {
    $0 = $1 >>> 0 >= 0 ? 0 : 1
   } else {
    $0 = 0
   }
  }
  HEAP32[19854] = $0 ? -1 : $4;
  HEAP32[19855] = $0 ? -1 : $5;
  HEAP32[19856] = $2;
  HEAP32[19857] = $3;
  HEAP32[3323] = HEAP32[20745] + 1251;
  HEAP32[3324] = 0;
  HEAP32[3052] = 0;
  HEAP32[3076] = 0;
  HEAP32[3077] = 0;
  HEAP32[3074] = -1048576;
  HEAP32[3075] = -1;
  HEAP32[3072] = -1048576;
  HEAP32[3073] = -1;
  init_rt_rq();
  init_dl_rq();
  $0 = HEAP32[19837];
  HEAP32[3292] = HEAP32[19836];
  HEAP32[3293] = $0;
  $0 = 4;
  while (1) {
   HEAP32[$0 + 12208 >> 2] = 0;
   $0 = $0 + 4 | 0;
   if (($0 | 0) != 24) {
    continue
   }
   break;
  };
  HEAP32[3322] = 0;
  label$2 : {
   if (HEAP32[340] == 5) {
    $0 = 3;
    HEAP32[268] = 3;
    $1 = 1431655765;
    break label$2;
   }
   $1 = HEAP32[263] + -100 << 2;
   $0 = HEAP32[$1 + 22208 >> 2];
   HEAP32[268] = $0;
   $1 = HEAP32[$1 + 22368 >> 2];
  }
  HEAP32[270] = $0;
  HEAP32[269] = $1;
  HEAP32[3945] = HEAP32[3945] + 1;
  init_idle(HEAP32[2]);
  HEAP32[19829] = HEAP32[20745] + 1251;
  HEAP32[19827] = 1;
 }
 
 function sched_clock() {
  return __wasm_i64_mul(HEAP32[20745] + 75e3 | 0, 0, 4e6, 0);
 }
 
 function sched_clock_cpu() {
  if (HEAP32[19830] < 1) {
   i64toi32_i32$HIGH_BITS = 0;
   return 0;
  }
  return sched_clock();
 }
 
 function dequeue_task_idle($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  HEAP8[79324] = 1;
  printk(22684, 0);
  dump_stack();
  HEAP8[79324] = 0;
 }
 
 function check_preempt_curr_idle($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
 }
 
 function pick_next_task_idle($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  FUNCTION_TABLE[HEAP32[HEAP32[$1 + 40 >> 2] + 28 >> 2]]($0, $1);
  return HEAP32[$0 + 1040 >> 2];
 }
 
 function put_prev_task_idle($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
 }
 
 function task_tick_idle($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
 }
 
 function switched_to_idle($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 + 8 >> 2] = 22763;
  HEAP32[$0 + 4 >> 2] = 421;
  HEAP32[$0 >> 2] = 22756;
  printk(22725, $0);
  abort();
 }
 
 function prio_changed_idle($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 + 8 >> 2] = 22780;
  HEAP32[$0 + 4 >> 2] = 427;
  HEAP32[$0 >> 2] = 22756;
  printk(22725, $0);
  abort();
 }
 
 function get_rr_interval_idle($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  return 0;
 }
 
 function update_curr() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  label$1 : {
   $6 = HEAP32[3078];
   if (!$6) {
    break label$1
   }
   $0 = HEAP32[3321];
   $1 = HEAP32[3320];
   $4 = HEAP32[$6 + 40 >> 2];
   $3 = $0 - (HEAP32[$6 + 44 >> 2] + ($1 >>> 0 < $4 >>> 0) | 0) | 0;
   $2 = $1 - $4 | 0;
   $4 = $2;
   if (($3 | 0) < 0) {
    $5 = 1
   } else {
    if (($3 | 0) <= 0) {
     $5 = $4 >>> 0 >= 1 ? 0 : 1
    } else {
     $5 = 0
    }
   }
   if ($5) {
    break label$1
   }
   $4 = $6 + 40 | 0;
   HEAP32[$4 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = $0;
   $4 = $3 + HEAP32[$6 + 52 >> 2] | 0;
   $0 = $2 + HEAP32[$6 + 48 >> 2] | 0;
   if ($0 >>> 0 < $2 >>> 0) {
    $4 = $4 + 1 | 0
   }
   HEAP32[$6 + 48 >> 2] = $0;
   HEAP32[$6 + 52 >> 2] = $4;
   $1 = HEAP32[$6 >> 2];
   label$2 : {
    if (($1 | 0) == 1024) {
     break label$2
    }
    $0 = HEAP32[$6 + 4 >> 2];
    if (!$0) {
     $5 = $6 + 4 | 0;
     if ($1) {
      $0 = 4294967295 / ($1 >>> 0) | 0
     } else {
      $0 = -1
     }
     HEAP32[$5 >> 2] = $0;
    }
    $1 = $0;
    $0 = $0 >>> 22 | 0;
    $5 = $1 << 10;
    $4 = 32;
    label$6 : {
     if ($1 >>> 0 >= 4194304) {
      while (1) {
       $4 = $4 + -1 | 0;
       $7 = ($0 | 0) == 1 & $5 >>> 0 > 4294967295 | $0 >>> 0 > 1;
       $1 = $0;
       $0 = $0 >>> 1 | 0;
       $1 = ($1 & 1) << 31 | $5 >>> 1;
       $5 = $1;
       if ($7) {
        continue
       }
       break;
      };
      break label$6;
     }
     $1 = $5;
    }
    $5 = $1;
    $0 = __wasm_i64_mul($1, 0, $2, 0);
    $7 = i64toi32_i32$HIGH_BITS;
    $2 = $0;
    $1 = $4 & 31;
    if (32 <= ($4 & 63) >>> 0) {
     $0 = 0;
     $1 = $7 >>> $1 | 0;
    } else {
     $0 = $7 >>> $1 | 0;
     $1 = ((1 << $1) - 1 & $7) << 32 - $1 | $2 >>> $1;
    }
    $2 = 0;
    if ($2 | $3) {
     $3 = __wasm_i64_mul($5, 0, $3, 0);
     $5 = i64toi32_i32$HIGH_BITS;
     $2 = $3;
     $4 = 32 - $4 | 0;
     $3 = $4 & 31;
     if (32 <= ($4 & 63) >>> 0) {
      $4 = $2 << $3;
      $2 = 0;
     } else {
      $4 = (1 << $3) - 1 & $2 >>> 32 - $3 | $5 << $3;
      $2 = $2 << $3;
     }
     $2 = $2 + $1 | 0;
     $3 = $0 + $4 | 0;
     $3 = $2 >>> 0 < $1 >>> 0 ? $3 + 1 | 0 : $3;
     break label$2;
    }
    $2 = $1;
    $3 = $0;
   }
   $0 = $6;
   $5 = $0;
   $3 = $3 + HEAP32[$0 + 60 >> 2] | 0;
   $1 = $2 + HEAP32[$0 + 56 >> 2] | 0;
   if ($1 >>> 0 < $2 >>> 0) {
    $3 = $3 + 1 | 0
   }
   HEAP32[$5 + 56 >> 2] = $1;
   HEAP32[$0 + 60 >> 2] = $3;
   $6 = HEAP32[3072];
   $5 = HEAP32[$0 + 32 >> 2];
   $0 = $5;
   $2 = $0 ? $1 : $6;
   $4 = HEAP32[3073];
   $3 = $0 ? $3 : $4;
   $0 = HEAP32[3077];
   label$10 : {
    if (!$0) {
     break label$10
    }
    $1 = $0 + 44 | 0;
    $0 = HEAP32[$1 >> 2];
    $1 = HEAP32[$1 + 4 >> 2];
    if ($5) {
     $8 = $0;
     $7 = $2;
     $5 = $1 - (($0 >>> 0 < $2 >>> 0) + $3 | 0) | 0;
     $2 = $0 - $2 | 0;
     if (($5 | 0) < 0) {
      $0 = 1
     } else {
      if (($5 | 0) <= 0) {
       $0 = $2 >>> 0 >= 0 ? 0 : 1
      } else {
       $0 = 0
      }
     }
     $2 = $0 ? $8 : $7;
     $3 = $0 ? $1 : $3;
     break label$10;
    }
    $2 = $0;
    $3 = $1;
   }
   $5 = $2;
   $1 = $6;
   $0 = $3 - (($2 >>> 0 < $6 >>> 0) + $4 | 0) | 0;
   $2 = $2 - $6 | 0;
   if (($0 | 0) > 0) {
    $2 = 1
   } else {
    if (($0 | 0) >= 0) {
     $2 = $2 >>> 0 <= 0 ? 0 : 1
    } else {
     $2 = 0
    }
   }
   HEAP32[3072] = $2 ? $5 : $1;
   HEAP32[3073] = $2 ? $3 : $4;
   $3 = HEAP32[3073];
   HEAP32[3074] = HEAP32[3072];
   HEAP32[3075] = $3;
  }
 }
 
 function enqueue_task_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  if (!HEAP32[$1 + 80 >> 2]) {
   $8 = $1 + 48 | 0;
   $9 = HEAP32[3078];
   $4 = $2 & 1;
   label$2 : {
    if ($4) {
     break label$2
    }
    if (($8 | 0) != ($9 | 0)) {
     break label$2
    }
    $3 = $1 + 104 | 0;
    $5 = $3;
    $7 = $3;
    $2 = HEAP32[$3 + 4 >> 2] + HEAP32[3073] | 0;
    $10 = HEAP32[$3 >> 2];
    $3 = HEAP32[3072];
    $6 = $10 + $3 | 0;
    if ($6 >>> 0 < $3 >>> 0) {
     $2 = $2 + 1 | 0
    }
    HEAP32[$7 >> 2] = $6;
    HEAP32[$5 + 4 >> 2] = $2;
   }
   update_curr();
   label$3 : {
    if ($4) {
     break label$3
    }
    if (($8 | 0) == ($9 | 0)) {
     break label$3
    }
    $3 = $1 + 104 | 0;
    $5 = $3;
    $7 = $3;
    $2 = HEAP32[$3 + 4 >> 2] + HEAP32[3073] | 0;
    $10 = HEAP32[$3 >> 2];
    $3 = HEAP32[3072];
    $6 = $10 + $3 | 0;
    if ($6 >>> 0 < $3 >>> 0) {
     $2 = $2 + 1 | 0
    }
    HEAP32[$7 >> 2] = $6;
    HEAP32[$5 + 4 >> 2] = $2;
   }
   HEAP32[3064] = HEAP32[3064] + HEAP32[$8 >> 2];
   HEAP32[3067] = HEAP32[3067] + 1;
   HEAP32[3058] = HEAP32[3058] + HEAP32[$8 >> 2];
   HEAP32[3065] = 0;
   HEAP32[3059] = 0;
   if ($4) {
    $5 = $1 + 104 | 0;
    $4 = $5;
    $11 = HEAP32[$4 + 4 >> 2];
    $12 = $11;
    $13 = $4;
    $3 = HEAP32[3072];
    $6 = HEAP32[3358];
    $2 = HEAP32[3073] - ($3 >>> 0 < $6 >>> 0) | 0;
    $3 = $3 - $6 | 0;
    $10 = $3;
    $5 = HEAP32[$4 >> 2];
    $7 = $5;
    $6 = $3 - $5 | 0;
    $3 = $2 - (($3 >>> 0 < $5 >>> 0) + $11 | 0) | 0;
    if (($3 | 0) > 0) {
     $3 = 1
    } else {
     if (($3 | 0) >= 0) {
      $3 = $6 >>> 0 <= 0 ? 0 : 1
     } else {
      $3 = 0
     }
    }
    HEAP32[$13 >> 2] = $3 ? $10 : $7;
    HEAP32[$4 + 4 >> 2] = $3 ? $2 : $12;
   }
   if (($8 | 0) != ($9 | 0)) {
    $5 = 0;
    $4 = HEAP32[3076];
    label$6 : {
     if ($4) {
      $2 = $1 + 104 | 0;
      $8 = HEAP32[$2 >> 2];
      $9 = HEAP32[$2 + 4 >> 2];
      $2 = 1;
      while (1) {
       $5 = $4;
       $4 = $4 + 44 | 0;
       $3 = HEAP32[$4 >> 2];
       $4 = $9 - (HEAP32[$4 + 4 >> 2] + ($8 >>> 0 < $3 >>> 0) | 0) | 0;
       $3 = $8 - $3 | 0;
       if (($4 | 0) < 0) {
        $7 = 1
       } else {
        if (($4 | 0) <= 0) {
         $7 = $3 >>> 0 >= 0 ? 0 : 1
        } else {
         $7 = 0
        }
       }
       $2 = $7 & $2;
       if (($4 | 0) > -1) {
        $3 = 1
       } else {
        if (($4 | 0) >= -1) {
         $3 = $3 >>> 0 <= 4294967295 ? 0 : 1
        } else {
         $3 = 0
        }
       }
       $4 = HEAP32[($3 ? 4 : 8) + $5 >> 2];
       if ($4) {
        continue
       }
       break;
      };
      $4 = $3 ? $5 + 4 | 0 : $5 + 8 | 0;
      break label$6;
     }
     $2 = 1;
     $4 = 12304;
    }
    $3 = $1 - -64 | 0;
    HEAP32[$3 >> 2] = 0;
    HEAP32[$3 + 4 >> 2] = 0;
    $3 = $1 + 60 | 0;
    HEAP32[$3 >> 2] = $5;
    HEAP32[$4 >> 2] = $3;
    rb_insert_color_cached($3, 12304, $2);
   }
   HEAP32[$1 + 80 >> 2] = 1;
  }
  HEAP32[3068] = HEAP32[3068] + 1;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
 }
 
 function dequeue_task_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  update_curr();
  $3 = $1 + 48 | 0;
  if (($3 | 0) == HEAP32[3080]) {
   HEAP32[3080] = 0
  }
  if (HEAP32[3079] == ($3 | 0)) {
   HEAP32[3079] = 0
  }
  if (HEAP32[3081] == ($3 | 0)) {
   HEAP32[3081] = 0
  }
  if (HEAP32[3078] != ($3 | 0)) {
   rb_erase_cached($1 + 60 | 0, 12304)
  }
  HEAP32[3064] = HEAP32[3064] - HEAP32[$1 + 48 >> 2];
  HEAP32[$1 + 80 >> 2] = 0;
  HEAP32[3067] = HEAP32[3067] + -1;
  HEAP32[3058] = HEAP32[3058] - HEAP32[$1 + 48 >> 2];
  HEAP32[3065] = 0;
  HEAP32[3059] = 0;
  if (!($2 & 1)) {
   $6 = $1 + 104 | 0;
   $3 = $6;
   $1 = HEAP32[$3 >> 2];
   $7 = HEAP32[3072];
   $3 = HEAP32[$3 + 4 >> 2] - (HEAP32[3073] + ($1 >>> 0 < $7 >>> 0) | 0) | 0;
   HEAP32[$6 >> 2] = $1 - $7;
   HEAP32[$6 + 4 >> 2] = $3;
  }
  if (($2 & 6) != 2) {
   $2 = 0;
   $6 = HEAP32[3072];
   $7 = HEAP32[3073];
   $5 = HEAP32[3077];
   $4 = HEAP32[3078];
   label$7 : {
    label$8 : {
     if ($4) {
      $3 = $6;
      $1 = $7;
      if (HEAP32[$4 + 32 >> 2]) {
       $3 = HEAP32[$4 + 56 >> 2];
       $2 = $4;
       $1 = HEAP32[$4 + 60 >> 2];
      }
      if ($5) {
       break label$8
      }
      break label$7;
     }
     $3 = $6;
     $1 = $7;
     if (!$5) {
      break label$7
     }
    }
    $4 = $5 + 44 | 0;
    $5 = HEAP32[$4 >> 2];
    $4 = HEAP32[$4 + 4 >> 2];
    if ($2) {
     $8 = $5;
     $9 = $3;
     $2 = $5 - $3 | 0;
     $3 = $4 - (($5 >>> 0 < $3 >>> 0) + $1 | 0) | 0;
     if (($3 | 0) < 0) {
      $2 = 1
     } else {
      if (($3 | 0) <= 0) {
       $2 = $2 >>> 0 >= 0 ? 0 : 1
      } else {
       $2 = 0
      }
     }
     $3 = $2 ? $8 : $9;
     $1 = $2 ? $4 : $1;
     break label$7;
    }
    $3 = $5;
    $1 = $4;
   }
   $5 = $3;
   $8 = $6;
   $2 = $3 - $6 | 0;
   $4 = $1 - (($3 >>> 0 < $6 >>> 0) + $7 | 0) | 0;
   if (($4 | 0) > 0) {
    $2 = 1
   } else {
    if (($4 | 0) >= 0) {
     $2 = $2 >>> 0 <= 0 ? 0 : 1
    } else {
     $2 = 0
    }
   }
   HEAP32[3072] = $2 ? $5 : $8;
   HEAP32[3073] = $2 ? $1 : $7;
   $1 = HEAP32[3073];
   HEAP32[3074] = HEAP32[3072];
   HEAP32[3075] = $1;
  }
  HEAP32[3068] = HEAP32[3068] + -1;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
 }
 
 function yield_task_fair($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  if (HEAP32[$0 >> 2] != 1) {
   $2 = HEAP32[$0 + 1036 >> 2];
   $1 = $2 + 48 | 0;
   if (($1 | 0) == HEAP32[3080]) {
    HEAP32[3080] = 0
   }
   if (($1 | 0) == HEAP32[3079]) {
    HEAP32[3079] = 0
   }
   if (($1 | 0) == HEAP32[3081]) {
    HEAP32[3081] = 0
   }
   if (HEAP32[$2 + 336 >> 2] != 3) {
    update_rq_clock($0);
    update_curr();
    HEAP32[$0 + 1056 >> 2] = HEAP32[$0 + 1056 >> 2] | 1;
   }
   HEAP32[3081] = $1;
  }
 }
 
 function yield_to_task_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  if (HEAP32[$1 + 80 >> 2]) {
   $1 = $1 + 48 | 0;
   if (HEAP32[$1 + 288 >> 2] != 5) {
    HEAP32[3079] = $1
   }
   if (HEAP32[$0 >> 2] != 1) {
    $2 = HEAP32[$0 + 1036 >> 2];
    $1 = $2 + 48 | 0;
    if (($1 | 0) == HEAP32[3080]) {
     HEAP32[3080] = 0
    }
    if (HEAP32[3079] == ($1 | 0)) {
     HEAP32[3079] = 0
    }
    if (HEAP32[3081] == ($1 | 0)) {
     HEAP32[3081] = 0
    }
    if (HEAP32[$2 + 336 >> 2] != 3) {
     update_rq_clock($0);
     update_curr();
     HEAP32[$0 + 1056 >> 2] = HEAP32[$0 + 1056 >> 2] | 1;
    }
    HEAP32[3081] = $1;
   }
   return 1;
  }
  return 0;
 }
 
 function check_preempt_wakeup($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  label$1 : {
   $2 = HEAP32[$0 + 1036 >> 2];
   if (($2 | 0) == ($1 | 0)) {
    break label$1
   }
   if (HEAP32[HEAP32[$2 + 4 >> 2] >> 2] & 8) {
    break label$1
   }
   if (HEAP32[$2 + 336 >> 2] != 5) {
    break label$1
   }
   if (HEAP32[$1 + 336 >> 2] == 5) {
    break label$1
   }
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function pick_next_task_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      if (HEAP32[$0 + 60 >> 2]) {
       FUNCTION_TABLE[HEAP32[HEAP32[$1 + 40 >> 2] + 28 >> 2]]($0, $1);
       $1 = HEAP32[$0 + 100 >> 2];
       $5 = $1 ? $1 + -12 | 0 : 0;
       $2 = ($5 | 0) != HEAP32[$0 + 116 >> 2];
       $1 = $2 ? $5 : 0;
       if ($2) {
        break label$1
       }
       if (!$5) {
        break label$1
       }
       $1 = $5;
       $2 = rb_next($1 + 12 | 0);
       $9 = $2 ? $2 + -12 | 0 : 0;
       if (!$9) {
        break label$1
       }
       $1 = HEAP32[$9 + 56 >> 2];
       $2 = HEAP32[$5 + 56 >> 2];
       $10 = $1 - $2 | 0;
       $1 = HEAP32[$9 + 60 >> 2] - (HEAP32[$5 + 60 >> 2] + ($1 >>> 0 < $2 >>> 0) | 0) | 0;
       $11 = $1;
       if (($1 | 0) < 0) {
        $1 = 1
       } else {
        if (($1 | 0) <= 0) {
         $1 = $10 >>> 0 >= 1 ? 0 : 1
        } else {
         $1 = 0
        }
       }
       if ($1) {
        break label$2
       }
       $8 = HEAP32[3360];
       $1 = HEAP32[$5 >> 2];
       if (($1 | 0) != 1024) {
        break label$4
       }
       break label$3;
      }
      return 0;
     }
     $4 = HEAP32[$5 + 4 >> 2];
     if (!$4) {
      $2 = $5 + 4 | 0;
      if ($1) {
       $4 = 4294967295 / ($1 >>> 0) | 0
      } else {
       $4 = -1
      }
      HEAP32[$2 >> 2] = $4;
     }
     $2 = $4 >>> 22 | 0;
     $7 = $4 << 10;
     $3 = 32;
     label$9 : {
      if ($4 >>> 0 >= 4194304) {
       $1 = 32;
       while (1) {
        $1 = $1 + -1 | 0;
        $4 = ($2 | 0) == 1 & $7 >>> 0 > 4294967295 | $2 >>> 0 > 1;
        $3 = $2;
        $2 = $2 >>> 1 | 0;
        $6 = ($3 & 1) << 31 | $7 >>> 1;
        $7 = $6;
        if ($4) {
         continue
        }
        break;
       };
       $3 = $1;
       break label$9;
      }
      $6 = $7;
     }
     $1 = __wasm_i64_mul($6, 0, $8, 0);
     $2 = i64toi32_i32$HIGH_BITS;
     $4 = $1;
     $1 = $3 & 31;
     if (32 <= ($3 & 63) >>> 0) {
      $8 = $2 >>> $1 | 0
     } else {
      $8 = ((1 << $1) - 1 & $2) << 32 - $1 | $4 >>> $1
     }
    }
    $1 = $5;
    if (($11 | 0) > 0) {
     $2 = 1
    } else {
     $2 = ($11 | 0) >= 0 ? ($10 >>> 0 <= $8 >>> 0 ? 0 : 1) : 0
    }
    if ($2) {
     break label$1
    }
   }
   $1 = $9;
  }
  $9 = HEAP32[$0 + 112 >> 2];
  label$12 : {
   if (!$9) {
    break label$12
   }
   $2 = HEAP32[$9 + 56 >> 2];
   $3 = HEAP32[$5 + 56 >> 2];
   $10 = $2 - $3 | 0;
   $2 = HEAP32[$9 + 60 >> 2] - (HEAP32[$5 + 60 >> 2] + ($2 >>> 0 < $3 >>> 0) | 0) | 0;
   $11 = $2;
   if (($2 | 0) < 0) {
    $2 = 1
   } else {
    if (($2 | 0) <= 0) {
     $2 = $10 >>> 0 >= 1 ? 0 : 1
    } else {
     $2 = 0
    }
   }
   if (!$2) {
    $8 = HEAP32[3360];
    $3 = HEAP32[$5 >> 2];
    if (($3 | 0) != 1024) {
     $2 = HEAP32[$5 + 4 >> 2];
     if (!$2) {
      $6 = $5 + 4 | 0;
      if ($3) {
       $2 = 4294967295 / ($3 >>> 0) | 0
      } else {
       $2 = -1
      }
      HEAP32[$6 >> 2] = $2;
     }
     $4 = $2;
     $2 = $2 >>> 22 | 0;
     $7 = $4 << 10;
     $3 = 32;
     label$18 : {
      if ($4 >>> 0 >= 4194304) {
       $4 = 32;
       while (1) {
        $4 = $4 + -1 | 0;
        $12 = ($2 | 0) == 1 & $7 >>> 0 > 4294967295 | $2 >>> 0 > 1;
        $3 = $2;
        $2 = $2 >>> 1 | 0;
        $6 = ($3 & 1) << 31 | $7 >>> 1;
        $7 = $6;
        if ($12) {
         continue
        }
        break;
       };
       $3 = $4;
       break label$18;
      }
      $6 = $7;
     }
     $2 = __wasm_i64_mul($6, 0, $8, 0);
     $4 = i64toi32_i32$HIGH_BITS;
     $7 = $2;
     $2 = $3 & 31;
     if (32 <= ($3 & 63) >>> 0) {
      $8 = $4 >>> $2 | 0
     } else {
      $8 = ((1 << $2) - 1 & $4) << 32 - $2 | $7 >>> $2
     }
    }
    if (($11 | 0) > 0) {
     $2 = 1
    } else {
     $2 = ($11 | 0) >= 0 ? ($10 >>> 0 <= $8 >>> 0 ? 0 : 1) : 0
    }
    if ($2) {
     break label$12
    }
   }
   $1 = $9;
  }
  $4 = HEAP32[$0 + 108 >> 2];
  label$21 : {
   if (!$4) {
    break label$21
   }
   $2 = HEAP32[$4 + 56 >> 2];
   $3 = HEAP32[$5 + 56 >> 2];
   $10 = $2 - $3 | 0;
   $2 = HEAP32[$4 + 60 >> 2] - (HEAP32[$5 + 60 >> 2] + ($2 >>> 0 < $3 >>> 0) | 0) | 0;
   $11 = $2;
   if (($2 | 0) < 0) {
    $2 = 1
   } else {
    if (($2 | 0) <= 0) {
     $2 = $10 >>> 0 >= 1 ? 0 : 1
    } else {
     $2 = 0
    }
   }
   if (!$2) {
    $8 = HEAP32[3360];
    $3 = HEAP32[$5 >> 2];
    if (($3 | 0) != 1024) {
     $2 = HEAP32[$5 + 4 >> 2];
     if (!$2) {
      $6 = $5 + 4 | 0;
      if ($3) {
       $2 = 4294967295 / ($3 >>> 0) | 0
      } else {
       $2 = -1
      }
      HEAP32[$6 >> 2] = $2;
     }
     $6 = $2;
     $2 = $2 >>> 22 | 0;
     $7 = $6 << 10;
     $3 = 32;
     label$27 : {
      if ($6 >>> 0 >= 4194304) {
       while (1) {
        $3 = $3 + -1 | 0;
        $5 = ($2 | 0) == 1 & $7 >>> 0 > 4294967295 | $2 >>> 0 > 1;
        $6 = $2;
        $2 = $2 >>> 1 | 0;
        $6 = ($6 & 1) << 31 | $7 >>> 1;
        $7 = $6;
        if ($5) {
         continue
        }
        break;
       };
       break label$27;
      }
      $6 = $7;
     }
     $2 = __wasm_i64_mul($6, 0, $8, 0);
     $7 = i64toi32_i32$HIGH_BITS;
     $8 = $2;
     $2 = $3 & 31;
     if (32 <= ($3 & 63) >>> 0) {
      $8 = $7 >>> $2 | 0
     } else {
      $8 = ((1 << $2) - 1 & $7) << 32 - $2 | $8 >>> $2
     }
    }
    if (($11 | 0) > 0) {
     $2 = 1
    } else {
     $2 = ($11 | 0) >= 0 ? ($10 >>> 0 <= $8 >>> 0 ? 0 : 1) : 0
    }
    if ($2) {
     break label$21
    }
   }
   $1 = $4;
  }
  label$30 : {
   if (($1 | 0) != ($9 | 0)) {
    break label$30
   }
   if (!$9) {
    break label$30
   }
   if (($9 | 0) != HEAP32[3080]) {
    break label$30
   }
   HEAP32[3080] = 0;
   $4 = HEAP32[$0 + 108 >> 2];
  }
  label$31 : {
   if (($1 | 0) != ($4 | 0)) {
    break label$31
   }
   if (!$1) {
    break label$31
   }
   if (HEAP32[3079] != ($1 | 0)) {
    break label$31
   }
   HEAP32[3079] = 0;
  }
  label$32 : {
   if (HEAP32[$0 + 116 >> 2] != ($1 | 0)) {
    break label$32
   }
   if (!$1) {
    break label$32
   }
   if (HEAP32[3081] != ($1 | 0)) {
    break label$32
   }
   HEAP32[3081] = 0;
  }
  if (HEAP32[$1 + 32 >> 2]) {
   rb_erase_cached($1 + 12 | 0, $0 + 96 | 0)
  }
  HEAP32[$0 + 104 >> 2] = $1;
  $2 = HEAP32[$1 + 52 >> 2];
  HEAP32[$1 + 64 >> 2] = HEAP32[$1 + 48 >> 2];
  HEAP32[$1 + 68 >> 2] = $2;
  $0 = $0 + 1072 | 0;
  $2 = HEAP32[$0 + 4 >> 2];
  HEAP32[$1 + 40 >> 2] = HEAP32[$0 >> 2];
  HEAP32[$1 + 44 >> 2] = $2;
  return $1 + -48 | 0;
 }
 
 function put_prev_task_fair($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $0 = $1 + 80 | 0;
  label$1 : {
   if (!HEAP32[$0 >> 2]) {
    break label$1
   }
   update_curr();
   if (!HEAP32[$0 >> 2]) {
    break label$1
   }
   $0 = HEAP32[3076];
   label$2 : {
    if ($0) {
     $2 = $1 + 104 | 0;
     $6 = HEAP32[$2 >> 2];
     $7 = HEAP32[$2 + 4 >> 2];
     $3 = 1;
     while (1) {
      $4 = $0;
      $0 = $0 + 44 | 0;
      $2 = HEAP32[$0 >> 2];
      $0 = $7 - (($6 >>> 0 < $2 >>> 0) + HEAP32[$0 + 4 >> 2] | 0) | 0;
      $2 = $6 - $2 | 0;
      if (($0 | 0) < 0) {
       $5 = 1
      } else {
       if (($0 | 0) <= 0) {
        $5 = $2 >>> 0 >= 0 ? 0 : 1
       } else {
        $5 = 0
       }
      }
      $3 = $5 & $3;
      if (($0 | 0) > -1) {
       $0 = 1
      } else {
       if (($0 | 0) >= -1) {
        $0 = $2 >>> 0 <= 4294967295 ? 0 : 1
       } else {
        $0 = 0
       }
      }
      $2 = $0;
      $0 = HEAP32[($2 ? 4 : 8) + $4 >> 2];
      if ($0) {
       continue
      }
      break;
     };
     $0 = $2 ? $4 + 4 | 0 : $4 + 8 | 0;
     break label$2;
    }
    $3 = 1;
    $0 = 12304;
   }
   $2 = $1 - -64 | 0;
   HEAP32[$2 >> 2] = 0;
   HEAP32[$2 + 4 >> 2] = 0;
   $1 = $1 + 60 | 0;
   HEAP32[$1 >> 2] = $4;
   HEAP32[$0 >> 2] = $1;
   rb_insert_color_cached($1, 12304, $3);
  }
  HEAP32[3078] = 0;
 }
 
 function set_curr_task_fair($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $0 = HEAP32[$0 + 1036 >> 2];
  $1 = $0 + 48 | 0;
  if (HEAP32[$0 + 80 >> 2]) {
   rb_erase_cached($0 + 60 | 0, 12304)
  }
  HEAP32[3078] = $1;
  $2 = HEAP32[3321];
  $1 = $0 + 88 | 0;
  HEAP32[$1 >> 2] = HEAP32[3320];
  HEAP32[$1 + 4 >> 2] = $2;
  $1 = $0 + 112 | 0;
  $0 = $0 + 96 | 0;
  $2 = HEAP32[$0 + 4 >> 2];
  HEAP32[$1 >> 2] = HEAP32[$0 >> 2];
  HEAP32[$1 + 4 >> 2] = $2;
 }
 
 function task_tick_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  update_curr();
  $0 = HEAP32[3067];
  label$1 : {
   if ($0 >>> 0 < 2) {
    break label$1
   }
   $8 = $1 + 48 | 0;
   $9 = HEAP32[3359];
   $2 = HEAP32[$1 + 80 >> 2];
   $0 = $0 + !$2 | 0;
   $6 = $0 >>> 0 > 8 ? Math_imul($9, $0) : HEAP32[3358];
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        if ($2) {
         $4 = HEAP32[$8 >> 2];
         $3 = HEAP32[3065];
         if (!$3) {
          break label$6
         }
         break label$2;
        }
        $4 = HEAP32[$8 >> 2];
        HEAP32[$7 + 4 >> 2] = 0;
        HEAP32[$7 + 8 >> 2] = HEAP32[3064] + $4;
        $2 = $7 + 4 | 0;
        $3 = HEAP32[$7 + 8 >> 2];
        if ($3) {
         break label$5
        }
        break label$4;
       }
       $2 = 12260;
       $3 = HEAP32[3064];
       if (!$3) {
        break label$4
       }
      }
      $3 = 4294967295 / ($3 >>> 0) | 0;
      break label$3;
     }
     $3 = -1;
    }
    HEAP32[$2 >> 2] = $3;
   }
   $2 = 32;
   $4 = __wasm_i64_mul($3, 0, $4, 0);
   $5 = i64toi32_i32$HIGH_BITS;
   $0 = $5;
   label$8 : {
    if (!(($0 | 0) == 1 & $4 >>> 0 < 0 | $0 >>> 0 < 1)) {
     $3 = 32;
     while (1) {
      $3 = $3 + -1 | 0;
      $10 = ($0 | 0) == 1 & $4 >>> 0 > 4294967295 | $0 >>> 0 > 1;
      $2 = $0;
      $0 = $0 >>> 1 | 0;
      $5 = ($2 & 1) << 31 | $4 >>> 1;
      $4 = $5;
      if ($10) {
       continue
      }
      break;
     };
     $2 = $3;
     break label$8;
    }
    $5 = $4;
   }
   $3 = HEAP32[$1 + 96 >> 2] - HEAP32[$1 + 112 >> 2] | 0;
   $0 = __wasm_i64_mul($5, 0, $6, 0);
   $6 = i64toi32_i32$HIGH_BITS;
   $5 = $2 & 31;
   $4 = $3;
   if (32 <= ($2 & 63) >>> 0) {
    $5 = $6 >>> $5 | 0
   } else {
    $5 = ((1 << $5) - 1 & $6) << 32 - $5 | $0 >>> $5
   }
   if ($4 >>> 0 > $5 >>> 0) {
    $1 = HEAP32[HEAP32[3311] + 4 >> 2];
    $0 = HEAP32[$1 >> 2];
    if (!($0 & 8)) {
     HEAP32[$1 >> 2] = $0 | 8
    }
    if (($8 | 0) == HEAP32[3080]) {
     HEAP32[3080] = 0
    }
    if (($8 | 0) == HEAP32[3079]) {
     HEAP32[3079] = 0
    }
    if (HEAP32[3081] != ($8 | 0)) {
     break label$1
    }
    HEAP32[3081] = 0;
    break label$1;
   }
   if ($9 >>> 0 > $3 >>> 0) {
    break label$1
   }
   $6 = $1 + 104 | 0;
   $2 = HEAP32[$6 >> 2];
   $0 = HEAP32[3077];
   $1 = $0 ? $0 + -12 | 0 : 0;
   $0 = HEAP32[$1 + 56 >> 2];
   $4 = $2 - $0 | 0;
   $1 = HEAP32[$6 + 4 >> 2] - (HEAP32[$1 + 60 >> 2] + ($2 >>> 0 < $0 >>> 0) | 0) | 0;
   $0 = $1;
   if (($0 | 0) < 0) {
    $1 = 1
   } else {
    if (($1 | 0) <= 0) {
     $1 = $4 >>> 0 >= 0 ? 0 : 1
    } else {
     $1 = 0
    }
   }
   if ($1) {
    break label$1
   }
   if (($0 | 0) < 0) {
    $0 = 1
   } else {
    $0 = ($0 | 0) <= 0 ? ($4 >>> 0 > $5 >>> 0 ? 0 : 1) : 0
   }
   if ($0) {
    break label$1
   }
   $1 = HEAP32[HEAP32[3311] + 4 >> 2];
   $0 = HEAP32[$1 >> 2];
   if (!($0 & 8)) {
    HEAP32[$1 >> 2] = $0 | 8
   }
  }
  global$0 = $7 + 16 | 0;
 }
 
 function task_fork_fair($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  update_rq_clock(12208);
  $4 = HEAP32[3078];
  label$1 : {
   if ($4) {
    update_curr();
    $2 = HEAP32[$4 + 60 >> 2];
    $5 = $0 + 104 | 0;
    $1 = HEAP32[$4 + 56 >> 2];
    HEAP32[$5 >> 2] = $1;
    HEAP32[$5 + 4 >> 2] = $2;
    break label$1;
   }
   $2 = $0 + 104 | 0;
   $1 = HEAP32[$2 >> 2];
   $2 = HEAP32[$2 + 4 >> 2];
  }
  $8 = $0 + 104 | 0;
  $6 = $8;
  $5 = HEAP32[3072];
  $9 = $5;
  $11 = $1;
  $3 = $5 - $1 | 0;
  $7 = HEAP32[3073];
  $10 = $7;
  $1 = $7 - (($5 >>> 0 < $1 >>> 0) + $2 | 0) | 0;
  if (($1 | 0) > 0) {
   $3 = 1
  } else {
   if (($1 | 0) >= 0) {
    $3 = $3 >>> 0 <= 0 ? 0 : 1
   } else {
    $3 = 0
   }
  }
  $1 = $3 ? $9 : $11;
  $3 = $3 ? $7 : $2;
  $2 = $3;
  HEAP32[$6 >> 2] = $1;
  HEAP32[$6 + 4 >> 2] = $2;
  label$3 : {
   if (!$4) {
    break label$3
   }
   if (!HEAP32[19832]) {
    break label$3
   }
   $3 = HEAP32[$4 + 60 >> 2];
   $7 = HEAP32[$4 + 56 >> 2];
   $6 = $3 - (($7 >>> 0 < $1 >>> 0) + $2 | 0) | 0;
   $9 = $7 - $1 | 0;
   if (($6 | 0) > -1) {
    $6 = 1
   } else {
    if (($6 | 0) >= -1) {
     $6 = $9 >>> 0 <= 4294967295 ? 0 : 1
    } else {
     $6 = 0
    }
   }
   if ($6) {
    break label$3
   }
   $4 = $4 + 56 | 0;
   HEAP32[$4 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = $2;
   $0 = $0 + 104 | 0;
   HEAP32[$0 >> 2] = $7;
   HEAP32[$0 + 4 >> 2] = $3;
   $1 = HEAP32[HEAP32[3311] + 4 >> 2];
   $2 = HEAP32[$1 >> 2];
   if (!($2 & 8)) {
    HEAP32[$1 >> 2] = $2 | 8
   }
   $1 = HEAP32[$0 >> 2];
   $2 = HEAP32[$0 + 4 >> 2];
   $5 = HEAP32[3072];
   $10 = HEAP32[3073];
  }
  HEAP32[$8 >> 2] = $1 - $5;
  HEAP32[$8 + 4 >> 2] = $2 - (($1 >>> 0 < $5 >>> 0) + $10 | 0);
 }
 
 function switched_from_fair($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2]) {
    break label$1
   }
   $0 = $1 + 96 | 0;
   if (!(HEAP32[$0 >> 2] | HEAP32[$0 + 4 >> 2])) {
    break label$1
   }
   if (HEAPU8[$1 + 436 | 0] & 8 ? HEAP32[$1 >> 2] == 512 : 0) {
    break label$1
   }
   $2 = $1 + 104 | 0;
   $6 = $2;
   $7 = $2;
   $0 = HEAP32[3072];
   $4 = HEAP32[3358];
   $1 = $0 - $4 | 0;
   $8 = $1;
   $3 = HEAP32[$2 >> 2];
   $9 = $3;
   $10 = $1 - $3 | 0;
   $5 = HEAP32[3073];
   $4 = $5 - ($0 >>> 0 < $4 >>> 0) | 0;
   $2 = HEAP32[$2 + 4 >> 2];
   $1 = $4 - ($2 + ($1 >>> 0 < $3 >>> 0) | 0) | 0;
   if (($1 | 0) > 0) {
    $1 = 1
   } else {
    if (($1 | 0) >= 0) {
     $1 = $10 >>> 0 <= 0 ? 0 : 1
    } else {
     $1 = 0
    }
   }
   $3 = $1 ? $8 : $9;
   HEAP32[$7 >> 2] = $3 - $0;
   HEAP32[$6 + 4 >> 2] = ($1 ? $4 : $2) - (($3 >>> 0 < $0 >>> 0) + $5 | 0);
  }
 }
 
 function switched_to_fair($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0;
  label$1 : {
   $2 = HEAP32[$1 + 20 >> 2];
   label$2 : {
    if (($2 | 0) != 1) {
     if ($2) {
      break label$2
     }
     $0 = $1 + 96 | 0;
     if (!(HEAP32[$0 >> 2] | HEAP32[$0 + 4 >> 2])) {
      break label$2
     }
     if (HEAPU8[$1 + 436 | 0] & 8 ? HEAP32[$1 >> 2] == 512 : 0) {
      break label$2
     }
     $1 = $1 + 104 | 0;
     $2 = $1;
     $4 = $1;
     $0 = HEAP32[$1 + 4 >> 2] + HEAP32[3073] | 0;
     $3 = HEAP32[3072];
     $1 = $3 + HEAP32[$1 >> 2] | 0;
     if ($1 >>> 0 < $3 >>> 0) {
      $0 = $0 + 1 | 0
     }
     HEAP32[$4 >> 2] = $1;
     HEAP32[$2 + 4 >> 2] = $0;
     return;
    }
    if (HEAP32[$0 + 1036 >> 2] == ($1 | 0)) {
     break label$1
    }
    check_preempt_curr($0, $1);
   }
   return;
  }
  $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
 }
 
 function prio_changed_fair($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2] != 1) {
    break label$1
   }
   if (($1 | 0) != HEAP32[$0 + 1036 >> 2]) {
    check_preempt_curr($0, $1);
    break label$1;
   }
   if (HEAP32[$1 + 24 >> 2] <= ($2 | 0)) {
    break label$1
   }
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function get_rr_interval_fair($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $2 = global$0 - 16 | 0;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        if (HEAP32[$0 + 48 >> 2]) {
         $3 = HEAP32[$1 + 80 >> 2];
         $0 = HEAP32[3067] + !$3 | 0;
         $5 = $0 >>> 0 > 8 ? Math_imul($0, HEAP32[3359]) : HEAP32[3358];
         if (!$3) {
          break label$6
         }
         $0 = HEAP32[$1 + 48 >> 2];
         $1 = HEAP32[3065];
         if (!$1) {
          break label$5
         }
         break label$1;
        }
        return 0;
       }
       $0 = HEAP32[$1 + 48 >> 2];
       HEAP32[$2 + 4 >> 2] = 0;
       HEAP32[$2 + 8 >> 2] = HEAP32[3064] + $0;
       $3 = $2 + 4 | 0;
       $1 = HEAP32[$2 + 8 >> 2];
       if ($1) {
        break label$4
       }
       break label$3;
      }
      $3 = 12260;
      $1 = HEAP32[3064];
      if (!$1) {
       break label$3
      }
     }
     $1 = 4294967295 / ($1 >>> 0) | 0;
     break label$2;
    }
    $1 = -1;
   }
   HEAP32[$3 >> 2] = $1;
  }
  $3 = 32;
  $4 = __wasm_i64_mul($1, 0, $0, 0);
  $1 = i64toi32_i32$HIGH_BITS;
  $0 = $1;
  label$8 : {
   if (!(($1 | 0) == 1 & $4 >>> 0 < 0 | $1 >>> 0 < 1)) {
    $1 = 32;
    while (1) {
     $1 = $1 + -1 | 0;
     $6 = ($0 | 0) == 1 & $4 >>> 0 > 4294967295 | $0 >>> 0 > 1;
     $3 = $0;
     $0 = $0 >>> 1 | 0;
     $2 = ($3 & 1) << 31 | $4 >>> 1;
     $4 = $2;
     if ($6) {
      continue
     }
     break;
    };
    $3 = $1;
    break label$8;
   }
   $2 = $4;
  }
  $0 = __wasm_i64_mul($2, 0, $5, 0);
  $1 = i64toi32_i32$HIGH_BITS;
  $2 = $3 & 31;
  if (32 <= ($3 & 63) >>> 0) {
   $0 = $1 >>> $2 | 0
  } else {
   $0 = ((1 << $2) - 1 & $1) << 32 - $2 | $0 >>> $2
  }
  return ($0 >>> 0) / 4e6 | 0;
 }
 
 function update_curr_fair($0) {
  $0 = $0 | 0;
  update_curr();
 }
 
 function init_rt_bandwidth($0, $1, $2, $3) {
  HEAP32[19836] = $2;
  HEAP32[19837] = $3;
  HEAP32[19834] = $0;
  HEAP32[19835] = $1;
  hrtimer_init(79352);
  HEAP32[19846] = 48;
 }
 
 function sched_rt_period_timer($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $7 = $0 + -16 | 0;
  $1 = $7;
  $3 = HEAP32[$1 >> 2];
  $1 = HEAP32[$1 + 4 >> 2];
  label$1 : {
   $2 = hrtimer_forward($0, FUNCTION_TABLE[HEAP32[HEAP32[$0 + 36 >> 2] + 28 >> 2]]() | 0, i64toi32_i32$HIGH_BITS, $3, $1);
   if ($2) {
    $8 = $7 + 8 | 0;
    $11 = $0 + 36 | 0;
    while (1) {
     if (!(HEAP32[3292] == -1 & HEAP32[3293] == -1)) {
      $1 = HEAP32[$8 + 4 >> 2];
      HEAP32[3292] = HEAP32[$8 >> 2];
      HEAP32[3293] = $1;
     }
     $3 = HEAP32[3286];
     label$5 : {
      label$6 : {
       label$7 : {
        if (!(HEAP32[3290] != 0 | HEAP32[3291] != 0)) {
         $1 = 1;
         if (!$3) {
          break label$7
         }
        }
        update_rq_clock(12208);
        label$9 : {
         label$10 : {
          label$11 : {
           label$12 : {
            label$13 : {
             label$14 : {
              label$15 : {
               if (HEAP32[3290] | HEAP32[3291]) {
                $1 = HEAP32[3291];
                $6 = HEAP32[3293];
                $5 = $6;
                $4 = HEAP32[3290];
                $9 = $4;
                $10 = HEAP32[3292];
                $6 = __wasm_i64_mul($10, $5, $2, $2 >> 31);
                $2 = i64toi32_i32$HIGH_BITS;
                $3 = ($2 | 0) == ($1 | 0) & $4 >>> 0 < $6 >>> 0 | $1 >>> 0 < $2 >>> 0;
                $4 = $3 ? $4 : $6;
                $2 = $1 - (($9 >>> 0 < $4 >>> 0) + ($3 ? $1 : $2) | 0) | 0;
                $4 = $9 - $4 | 0;
                HEAP32[3290] = $4;
                HEAP32[3291] = $2;
                if (!HEAP32[3289]) {
                 break label$15
                }
                if (($2 | 0) == ($5 | 0) & $4 >>> 0 >= $10 >>> 0 | $2 >>> 0 > $5 >>> 0) {
                 break label$15
                }
                $1 = 0;
                HEAP32[3289] = 0;
                $3 = HEAP32[3286];
                if (!$3) {
                 break label$12
                }
                $1 = $3;
                if (HEAP32[3311] == HEAP32[3312]) {
                 HEAP32[3316] = HEAP32[3316] & -2
                }
                $5 = 0;
                break label$11;
               }
               $5 = HEAP32[3289];
               $2 = HEAP32[3286];
               if (!$2) {
                break label$14
               }
               $6 = 0;
               $3 = 0;
               $1 = 0;
               $4 = $1;
               if ($5) {
                break label$13
               }
               break label$10;
              }
              $1 = HEAP32[3286];
              $5 = 1;
              break label$11;
             }
             $4 = 1;
            }
            $1 = $4;
            if ($5) {
             break label$6
            }
            break label$7;
           }
           $5 = 0;
          }
          $6 = HEAP32[3289];
          $3 = ($6 | 0) != 0;
          $1 = !($2 | $4 | $1);
          if ($5) {
           break label$9
          }
          $2 = HEAP32[3286];
          if (!$2) {
           break label$9
          }
         }
         if (!(HEAP32[3288] | $6)) {
          HEAP32[3288] = 1;
          HEAP32[3052] = HEAP32[3052] + $2;
         }
         $2 = HEAP32[HEAP32[3311] + 4 >> 2];
         $5 = HEAP32[$2 >> 2];
         if (!($5 & 8)) {
          HEAP32[$2 >> 2] = $5 | 8
         }
        }
        if ($3) {
         break label$6
        }
       }
       $3 = 1;
       if (HEAP32[3051] < 0) {
        break label$5
       }
       if (HEAP32[$8 >> 2] == -1 & HEAP32[$8 + 4 >> 2] == -1) {
        break label$5
       }
      }
      $3 = $1;
     }
     $1 = HEAP32[$7 >> 2];
     $2 = HEAP32[$7 + 4 >> 2];
     $2 = hrtimer_forward($0, FUNCTION_TABLE[HEAP32[HEAP32[$11 >> 2] + 28 >> 2]]() | 0, i64toi32_i32$HIGH_BITS, $1, $2);
     if ($2) {
      continue
     }
     break;
    };
    if (!$3) {
     break label$1
    }
    HEAP32[$7 + 64 >> 2] = 0;
    return 0;
   }
   return 1;
  }
  return 1;
 }
 
 function init_rt_rq() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = 12344;
  while (1) {
   HEAP32[$0 + 4 >> 2] = $0;
   $2 = ($1 >>> 3 & 28) + 12328 | 0;
   $3 = HEAP32[$2 >> 2];
   (wasm2js_i32$0 = $2, wasm2js_i32$1 = __wasm_rotl_i32(-2, $1) & $3), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
   HEAP32[$0 >> 2] = $0;
   $0 = $0 + 8 | 0;
   $1 = $1 + 1 | 0;
   if (($1 | 0) != 100) {
    continue
   }
   break;
  };
  HEAP32[3290] = 0;
  HEAP32[3291] = 0;
  HEAP32[3292] = 0;
  HEAP32[3293] = 0;
  HEAP32[3288] = 0;
  HEAP32[3289] = 0;
  HEAP32[3085] = HEAP32[3085] | 16;
 }
 
 function sched_rt_bandwidth_account($0) {
  var $1 = 0, $2 = 0;
  if (hrtimer_active(79352)) {
   $0 = 1
  } else {
   $1 = HEAP32[$0 + 836 >> 2];
   $2 = HEAP32[19837];
   $0 = ($1 | 0) == ($2 | 0) & HEAPU32[$0 + 832 >> 2] < HEAPU32[19836] | $1 >>> 0 < $2 >>> 0;
  }
  return $0;
 }
 
 function enqueue_task_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0;
  $3 = $1 + 128 | 0;
  if ($2 & 1) {
   HEAP32[$1 + 136 >> 2] = 0
  }
  dequeue_rt_stack($3, $2);
  if (($2 & 6) != 2) {
   $5 = $3 + -128 | 0;
   $4 = HEAP32[$3 + -104 >> 2];
   $0 = ($4 << 3) + 12344 | 0;
   label$3 : {
    if (!($2 & 16)) {
     $2 = ($4 << 3) + 12348 | 0;
     $4 = HEAP32[$2 >> 2];
     HEAP32[$4 >> 2] = $3;
     HEAP32[$2 >> 2] = $3;
     $2 = $0;
     $0 = $4;
     break label$3;
    }
    $2 = HEAP32[$0 >> 2];
    HEAP32[$2 + 4 >> 2] = $3;
    HEAP32[$0 >> 2] = $3;
   }
   HEAP32[$1 + 132 >> 2] = $0;
   HEAP32[$1 + 128 >> 2] = $2;
   HEAP16[$1 + 150 >> 1] = 1;
   $0 = HEAP32[$5 + 24 >> 2];
   $2 = (($0 | 0) / 32 << 2) + 12328 | 0;
   HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 1 << ($0 & 31);
  }
  HEAP16[$1 + 148 >> 1] = 1;
  HEAP32[3286] = HEAP32[3286] + 1;
  HEAP32[3287] = HEAP32[3287] + (HEAP32[$1 + 336 >> 2] == 2);
  label$5 : {
   if (HEAP32[3051] < 0) {
    break label$5
   }
   if (HEAP32[19836] == -1 & HEAP32[19837] == -1) {
    break label$5
   }
   if (!HEAP32[19850]) {
    HEAP32[19850] = 1;
    hrtimer_forward(79352, FUNCTION_TABLE[HEAP32[HEAP32[19847] + 28 >> 2]]() | 0, i64toi32_i32$HIGH_BITS, 0, 0);
    $0 = HEAP32[19842];
    $1 = HEAP32[19844];
    $2 = $1;
    $4 = $0 - $2 | 0;
    $1 = HEAP32[19845];
    hrtimer_start_range_ns(79352, $2, $1, $4, HEAP32[19843] - ($1 + ($0 >>> 0 < $2 >>> 0) | 0) | 0, 2);
   }
  }
  label$7 : {
   if (HEAP32[3289] | HEAP32[3288]) {
    break label$7
   }
   $0 = HEAP32[3286];
   if (!$0) {
    break label$7
   }
   HEAP32[3288] = 1;
   HEAP32[3052] = $0 + HEAP32[3052];
  }
 }
 
 function dequeue_rt_stack($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  if ($0) {
   HEAP32[$0 + 24 >> 2] = 0;
   dequeue_top_rt_rq();
   $3 = ($1 & 6) == 2;
   while (1) {
    if (HEAPU16[$0 + 20 >> 1]) {
     if (!$3) {
      $1 = HEAP32[$0 + 4 >> 2];
      $2 = HEAP32[$0 >> 2];
      HEAP32[$1 >> 2] = $2;
      HEAP32[$2 + 4 >> 2] = $1;
      HEAP32[$0 >> 2] = $0;
      HEAP32[$0 + 4 >> 2] = $0;
      $1 = HEAP32[$0 + -104 >> 2];
      $2 = ($1 << 3) + 12344 | 0;
      if (HEAP32[$2 >> 2] == ($2 | 0)) {
       $2 = (($1 | 0) / 32 << 2) + 12328 | 0;
       $4 = HEAP32[$2 >> 2];
       (wasm2js_i32$0 = $2, wasm2js_i32$1 = __wasm_rotl_i32(-2, $1) & $4), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
      }
      HEAP16[$0 + 22 >> 1] = 0;
     }
     HEAP16[$0 + 20 >> 1] = 0;
     HEAP32[3286] = HEAP32[3286] + -1;
     HEAP32[3287] = HEAP32[3287] - (HEAP32[$0 + 208 >> 2] == 2);
    }
    $0 = HEAP32[$0 + 24 >> 2];
    if ($0) {
     continue
    }
    break;
   };
   return;
  }
  dequeue_top_rt_rq();
 }
 
 function dequeue_task_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  update_curr_rt($0);
  dequeue_rt_stack($1 + 128 | 0, $2);
  label$1 : {
   if (HEAP32[3289] | HEAP32[3288]) {
    break label$1
   }
   $0 = HEAP32[3286];
   if (!$0) {
    break label$1
   }
   HEAP32[3288] = 1;
   HEAP32[3052] = $0 + HEAP32[3052];
  }
 }
 
 function update_curr_rt($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  $2 = HEAP32[$0 + 1036 >> 2];
  label$1 : {
   if (HEAP32[$2 + 40 >> 2] != 22868) {
    break label$1
   }
   $7 = HEAP32[$0 + 1076 >> 2];
   $8 = HEAP32[$0 + 1072 >> 2];
   $4 = $8;
   $3 = $2 + 88 | 0;
   $1 = $3;
   $5 = HEAP32[$1 >> 2];
   $1 = $7 - (($4 >>> 0 < $5 >>> 0) + HEAP32[$1 + 4 >> 2] | 0) | 0;
   $4 = $4 - $5 | 0;
   $5 = $4;
   if (($1 | 0) < 0) {
    $5 = 1
   } else {
    if (($1 | 0) <= 0) {
     $5 = $5 >>> 0 >= 1 ? 0 : 1
    } else {
     $5 = 0
    }
   }
   if ($5) {
    break label$1
   }
   HEAP32[$3 >> 2] = $8;
   HEAP32[$3 + 4 >> 2] = $7;
   $3 = $2 + 96 | 0;
   $7 = $3;
   $5 = $3;
   $2 = $1 + HEAP32[$3 + 4 >> 2] | 0;
   $3 = $4 + HEAP32[$3 >> 2] | 0;
   if ($3 >>> 0 < $4 >>> 0) {
    $2 = $2 + 1 | 0
   }
   HEAP32[$5 >> 2] = $3;
   HEAP32[$7 + 4 >> 2] = $2;
   if (HEAP32[3051] < 0) {
    break label$1
   }
   if (HEAP32[3292] == -1 & HEAP32[3293] == -1) {
    break label$1
   }
   $1 = $1 + HEAP32[3291] | 0;
   $2 = $4 + HEAP32[3290] | 0;
   if ($2 >>> 0 < $4 >>> 0) {
    $1 = $1 + 1 | 0
   }
   HEAP32[3290] = $2;
   HEAP32[3291] = $1;
   label$2 : {
    label$3 : {
     if (HEAP32[3289]) {
      break label$3
     }
     $4 = HEAP32[3293];
     $3 = $4;
     $7 = $2;
     $2 = HEAP32[3292];
     if (($3 | 0) == ($1 | 0) & $7 >>> 0 <= $2 >>> 0 | $1 >>> 0 < $3 >>> 0) {
      break label$2
     }
     $1 = HEAP32[19835];
     if (($1 | 0) == ($4 | 0) & $2 >>> 0 >= HEAPU32[19834] | $4 >>> 0 > $1 >>> 0) {
      break label$2
     }
     label$4 : {
      if (HEAP32[19836] | HEAP32[19837]) {
       HEAP32[3289] = 1;
       if (HEAPU8[79408]) {
        break label$4
       }
       HEAP8[79408] = 1;
       printk_deferred(23029, 0);
       if (HEAP32[3289]) {
        break label$4
       }
       break label$2;
      }
      HEAP32[3290] = 0;
      HEAP32[3291] = 0;
      break label$2;
     }
     if (!HEAP32[3288]) {
      break label$3
     }
     $1 = HEAP32[3052];
     if ($1) {
      HEAP32[3052] = $1 - HEAP32[3286];
      HEAP32[3288] = 0;
      break label$3;
     }
     HEAP32[$6 + 8 >> 2] = 22972;
     HEAP32[$6 + 4 >> 2] = 1004;
     HEAP32[$6 >> 2] = 22967;
     printk(22936, $6);
     abort();
    }
    $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
    $1 = HEAP32[$0 >> 2];
    if (!($1 & 8)) {
     HEAP32[$0 >> 2] = $1 | 8
    }
   }
  }
  global$0 = $6 + 16 | 0;
 }
 
 function yield_task_rt($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $0 = HEAP32[$0 + 1036 >> 2];
  if (HEAPU16[$0 + 148 >> 1]) {
   $1 = HEAP32[$0 + 128 >> 2];
   $5 = $0 + 132 | 0;
   $2 = HEAP32[$5 >> 2];
   HEAP32[$1 + 4 >> 2] = $2;
   $4 = $0 + 128 | 0;
   $3 = HEAP32[$4 + -104 >> 2];
   HEAP32[$2 >> 2] = $1;
   $2 = $3 << 3;
   $3 = $2 + 12348 | 0;
   $1 = HEAP32[$3 >> 2];
   HEAP32[$3 >> 2] = $4;
   HEAP32[$0 + 128 >> 2] = $2 + 12344;
   HEAP32[$1 >> 2] = $4;
   HEAP32[$5 >> 2] = $1;
  }
 }
 
 function check_preempt_curr_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  if (HEAP32[$1 + 24 >> 2] < HEAP32[HEAP32[$0 + 1036 >> 2] + 24 >> 2]) {
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function pick_next_task_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  if (HEAP32[$1 + 40 >> 2] == 22868) {
   update_curr_rt($0)
  }
  label$2 : {
   label$3 : {
    label$4 : {
     label$5 : {
      label$6 : {
       if (HEAP32[$0 + 944 >> 2]) {
        FUNCTION_TABLE[HEAP32[HEAP32[$1 + 40 >> 2] + 28 >> 2]]($0, $1);
        $1 = HEAP32[$0 + 120 >> 2];
        if (!$1) {
         break label$6
        }
        $5 = $1 & 65535;
        $1 = $5 ? $1 : $1 >>> 16 | 0;
        $3 = $1 & 255;
        $1 = $3 ? $1 : $1 >>> 8 | 0;
        $4 = $1 & 15;
        $1 = $4 ? $1 : $1 >>> 4 | 0;
        $6 = $1 & 3;
        $7 = (($6 ? $1 : $1 >>> 2 | 0) ^ -1) & 1;
        $1 = !$5 << 4;
        $1 = $3 ? $1 : $1 | 8;
        $1 = $4 ? $1 : $1 | 4;
        $1 = $7 + ($6 ? $1 : $1 | 2) | 0;
        break label$5;
       }
       $0 = 0;
       break label$4;
      }
      $1 = HEAP32[$0 + 124 >> 2];
      if ($1) {
       $3 = $1 & 65535;
       $4 = !$3 << 4;
       $1 = $3 ? $1 : $1 >>> 16 | 0;
       $3 = $1 & 255;
       $4 = $3 ? $4 : $4 | 8;
       $1 = $3 ? $1 : $1 >>> 8 | 0;
       $3 = $1 & 15;
       $4 = $3 ? $4 : $4 | 4;
       $1 = $3 ? $1 : $1 >>> 4 | 0;
       $3 = $1 & 3;
       $1 = (($3 ? $4 : $4 | 2) + ((($3 ? $1 : $1 >>> 2 | 0) ^ -1) & 1) | 0) + 32 | 0;
       break label$5;
      }
      $1 = HEAP32[$0 + 128 >> 2];
      if ($1) {
       $3 = $1 & 65535;
       $4 = !$3 << 4;
       $1 = $3 ? $1 : $1 >>> 16 | 0;
       $3 = $1 & 255;
       $4 = $3 ? $4 : $4 | 8;
       $1 = $3 ? $1 : $1 >>> 8 | 0;
       $3 = $1 & 15;
       $4 = $3 ? $4 : $4 | 4;
       $1 = $3 ? $1 : $1 >>> 4 | 0;
       $3 = $1 & 3;
       $1 = (($3 ? $4 : $4 | 2) + ((($3 ? $1 : $1 >>> 2 | 0) ^ -1) & 1) | 0) - -64 | 0;
       break label$5;
      }
      $1 = HEAP32[$0 + 132 >> 2];
      $3 = $1 & 65535;
      $4 = !$3 << 4;
      $1 = $3 ? $1 : $1 >>> 16 | 0;
      $3 = $1 & 255;
      $4 = $3 ? $4 : $4 | 8;
      $1 = $3 ? $1 : $1 >>> 8 | 0;
      $3 = $1 & 15;
      $4 = $3 ? $4 : $4 | 4;
      $1 = $3 ? $1 : $1 >>> 4 | 0;
      $3 = $1 & 3;
      $1 = (($3 ? $4 : $4 | 2) + ((($3 ? $1 : $1 >>> 2 | 0) ^ -1) & 1) | 0) + 96 | 0;
     }
     if (($1 | 0) >= 100) {
      break label$3
     }
     $1 = HEAP32[(($1 << 3) + $0 | 0) + 136 >> 2];
     if (!$1) {
      break label$2
     }
     $4 = HEAP32[$0 + 1076 >> 2];
     $3 = $1 + -40 | 0;
     HEAP32[$3 >> 2] = HEAP32[$0 + 1072 >> 2];
     HEAP32[$3 + 4 >> 2] = $4;
     $0 = $1 + -128 | 0;
    }
    global$0 = $2 + 32 | 0;
    return $0 | 0;
   }
   HEAP32[$2 + 8 >> 2] = 23009;
   HEAP32[$2 + 4 >> 2] = 1510;
   HEAP32[$2 >> 2] = 22967;
   printk(22936, $2);
   abort();
  }
  HEAP32[$2 + 24 >> 2] = 22990;
  HEAP32[$2 + 20 >> 2] = 1526;
  HEAP32[$2 + 16 >> 2] = 22967;
  printk(22936, $2 + 16 | 0);
  abort();
 }
 
 function put_prev_task_rt($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  update_curr_rt($0);
 }
 
 function set_curr_task_rt($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $2 = HEAP32[$0 + 1076 >> 2];
  $1 = HEAP32[$0 + 1036 >> 2] + 88 | 0;
  HEAP32[$1 >> 2] = HEAP32[$0 + 1072 >> 2];
  HEAP32[$1 + 4 >> 2] = $2;
 }
 
 function task_tick_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  update_curr_rt($0);
  label$1 : {
   if (HEAP32[$1 + 336 >> 2] != 2) {
    break label$1
   }
   $2 = $1 + 144 | 0;
   $3 = HEAP32[$2 >> 2] + -1 | 0;
   HEAP32[$2 >> 2] = $3;
   if ($3) {
    break label$1
   }
   HEAP32[$2 >> 2] = HEAP32[3361];
   $3 = $1 + 128 | 0;
   $2 = $3;
   while (1) {
    if (!$2) {
     break label$1
    }
    $5 = HEAP32[$2 >> 2];
    $4 = HEAP32[$2 + 4 >> 2];
    $2 = 0;
    if (($4 | 0) == ($5 | 0)) {
     continue
    }
    break;
   };
   if (HEAPU16[$1 + 148 >> 1]) {
    $2 = HEAP32[$1 + 128 >> 2];
    $5 = $1 + 132 | 0;
    $4 = HEAP32[$5 >> 2];
    HEAP32[$2 + 4 >> 2] = $4;
    $6 = HEAP32[$3 + -104 >> 2];
    HEAP32[$4 >> 2] = $2;
    $4 = $6 << 3;
    $6 = $4 + 12348 | 0;
    $2 = HEAP32[$6 >> 2];
    HEAP32[$6 >> 2] = $3;
    HEAP32[$1 + 128 >> 2] = $4 + 12344;
    HEAP32[$2 >> 2] = $3;
    HEAP32[$5 >> 2] = $2;
   }
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function switched_to_rt($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2] != 1) {
    break label$1
   }
   $2 = HEAP32[$0 + 1036 >> 2];
   if (($2 | 0) == ($1 | 0)) {
    break label$1
   }
   if (HEAP32[$1 + 24 >> 2] >= HEAP32[$2 + 24 >> 2]) {
    break label$1
   }
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function prio_changed_rt($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2] != 1) {
    break label$1
   }
   $3 = HEAP32[$1 + 24 >> 2];
   $4 = HEAP32[$0 + 1036 >> 2];
   label$2 : {
    if (($1 | 0) != ($4 | 0)) {
     if (($3 | 0) >= HEAP32[$4 + 24 >> 2]) {
      break label$1
     }
     break label$2;
    }
    if (($3 | 0) <= ($2 | 0)) {
     break label$1
    }
   }
   $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
   $1 = HEAP32[$0 >> 2];
   if (!($1 & 8)) {
    HEAP32[$0 >> 2] = $1 | 8
   }
  }
 }
 
 function get_rr_interval_rt($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  return (HEAP32[$1 + 336 >> 2] == 2 ? HEAP32[3361] : 0) | 0;
 }
 
 function dequeue_top_rt_rq() {
  var $0 = 0, $1 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  label$1 : {
   if (HEAP32[3288]) {
    $1 = HEAP32[3052];
    if (!$1) {
     break label$1
    }
    HEAP32[3052] = $1 - HEAP32[3286];
    HEAP32[3288] = 0;
   }
   global$0 = $0 + 16 | 0;
   return;
  }
  HEAP32[$0 + 8 >> 2] = 22972;
  HEAP32[$0 + 4 >> 2] = 1004;
  HEAP32[$0 >> 2] = 22967;
  printk(22936, $0);
  abort();
 }
 
 function init_dl_rq() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  HEAP32[3294] = 0;
  HEAP32[3295] = 0;
  $0 = HEAP32[3051];
  label$1 : {
   if (($0 | 0) >= 0) {
    $0 = to_ratio(__wasm_i64_mul(HEAP32[3050], 0, 1e3, 0), i64toi32_i32$HIGH_BITS, __wasm_i64_mul($0, $0 >> 31, 1e3, 0), i64toi32_i32$HIGH_BITS);
    break label$1;
   }
   $1 = -1;
   $0 = -1;
  }
  HEAP32[3298] = $0;
  HEAP32[3299] = $1;
  HEAP32[3304] = 0;
  HEAP32[3305] = 0;
  HEAP32[3302] = 0;
  HEAP32[3303] = 0;
  HEAP32[3300] = 0;
  HEAP32[3301] = 0;
  $0 = HEAP32[3051];
  if (($0 | 0) > -1) {
   (wasm2js_i32$0 = 13232, wasm2js_i32$1 = to_ratio(__wasm_i64_mul($0, $0 >> 31, 1e3, 0), i64toi32_i32$HIGH_BITS, __wasm_i64_mul(HEAP32[3050], 0, 1e3, 0), i64toi32_i32$HIGH_BITS) >>> 12 | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
   HEAP32[3309] = 0;
   $2 = __wasm_i64_mul(HEAP32[3050], 0, 1e3, 0);
   $3 = i64toi32_i32$HIGH_BITS;
   $1 = HEAP32[3051];
   $0 = $1 >> 31;
   $4 = __wasm_i64_mul($1, $0, 1e3, 0);
   $5 = i64toi32_i32$HIGH_BITS;
   if (($0 | 0) < 0) {
    $0 = 1
   } else {
    if (($0 | 0) <= 0) {
     $0 = $1 >>> 0 >= 0 ? 0 : 1
    } else {
     $0 = 0
    }
   }
   (wasm2js_i32$0 = 13224, wasm2js_i32$1 = to_ratio($2, $3, $0 ? -1 : $4, $0 ? -1 : $5)), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
   HEAP32[3307] = 0;
   return;
  }
  HEAP32[3308] = 256;
  HEAP32[3309] = 0;
  HEAP32[3306] = 1048576;
  HEAP32[3307] = 0;
 }
 
 function dl_task_timer($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $1 = $0 + -80 | 0;
  $4 = $0 + -240 | 0;
  $2 = task_rq_lock($4, $3 + 8 | 0);
  label$1 : {
   if (HEAP32[$0 + -216 >> 2] > -1) {
    break label$1
   }
   if ((HEAPU8[$1 + 76 | 0] & 3) != 1) {
    break label$1
   }
   update_rq_clock($2);
   if (HEAP32[$4 + 20 >> 2] == 1) {
    enqueue_task_dl($2, $4, 32);
    $0 = HEAP32[$2 + 1036 >> 2];
    if (HEAP32[$0 + 24 >> 2] <= -1) {
     $5 = HEAP32[$1 + 64 >> 2];
     $0 = $0 + 224 | 0;
     $6 = HEAP32[$0 >> 2];
     $0 = HEAP32[$1 + 68 >> 2] - (($5 >>> 0 < $6 >>> 0) + HEAP32[$0 + 4 >> 2] | 0) | 0;
     $5 = $5 - $6 | 0;
     if (($0 | 0) > -1) {
      $0 = 1
     } else {
      if (($0 | 0) >= -1) {
       $0 = $5 >>> 0 <= 4294967295 ? 0 : 1
      } else {
       $0 = 0
      }
     }
     if ($0) {
      break label$1
     }
    }
    $0 = HEAP32[HEAP32[$2 + 1036 >> 2] + 4 >> 2];
    $2 = HEAP32[$0 >> 2];
    if (!($2 & 8)) {
     HEAP32[$0 >> 2] = $2 | 8
    }
    break label$1;
   }
   replenish_dl_entity($1, $1);
  }
  HEAP32[19859] = HEAP32[$3 + 8 >> 2];
  $1 = $1 + -152 | 0;
  $0 = HEAP32[$1 >> 2] + -1 | 0;
  HEAP32[$1 >> 2] = $0;
  if (!$0) {
   __put_task_struct($4)
  }
  global$0 = $3 + 16 | 0;
  return 0;
 }
 
 function enqueue_task_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0;
  $9 = global$0 - 48 | 0;
  global$0 = $9;
  $10 = HEAPU8[$1 + 236 | 0];
  label$1 : {
   label$2 : {
    label$3 : {
     if (HEAP32[$1 + 32 >> 2] >= 0) {
      if (($2 | 0) != 32) {
       break label$3
      }
      if (!($10 & 2)) {
       break label$3
      }
      break label$1;
     }
     $11 = $1 + 160 | 0;
     if ($10 & 1) {
      break label$2
     }
     $3 = $1 + 184 | 0;
     $4 = HEAP32[$3 >> 2];
     $5 = $1 + 192 | 0;
     $7 = HEAP32[$5 >> 2];
     $12 = HEAP32[$3 + 4 >> 2];
     $13 = HEAP32[$5 + 4 >> 2];
     if (($4 | 0) == ($7 | 0) & ($12 | 0) == ($13 | 0)) {
      break label$2
     }
     $5 = $1 + 224 | 0;
     $3 = HEAP32[$5 >> 2];
     $14 = HEAP32[$5 + 4 >> 2];
     $15 = HEAP32[3319];
     $5 = HEAP32[3318];
     $6 = $14 - ($15 + ($3 >>> 0 < $5 >>> 0) | 0) | 0;
     $8 = $3 - $5 | 0;
     if (($6 | 0) > -1) {
      $8 = 1
     } else {
      if (($6 | 0) >= -1) {
       $8 = $8 >>> 0 <= 4294967295 ? 0 : 1
      } else {
       $8 = 0
      }
     }
     if ($8) {
      break label$2
     }
     $6 = $4 - $7 | 0;
     $8 = $5 + ($6 - $3 | 0) | 0;
     $6 = $15 + ($12 - (($4 >>> 0 < $7 >>> 0) + $13 | 0) - (($6 >>> 0 < $3 >>> 0) + $14) | 0) | 0;
     $3 = $8;
     $6 = $3 >>> 0 < $5 >>> 0 ? $6 + 1 | 0 : $6;
     if (($6 | 0) > -1) {
      $3 = 1
     } else {
      if (($6 | 0) >= -1) {
       $3 = $3 >>> 0 <= 4294967295 ? 0 : 1
      } else {
       $3 = 0
      }
     }
     if ($3) {
      break label$2
     }
     if ($10 & 2) {
      break label$2
     }
     if (!start_dl_timer($11 + -160 | 0)) {
      break label$2
     }
     $3 = $1 + 236 | 0;
     HEAP8[$3 | 0] = HEAPU8[$3 | 0] | 1;
     $3 = $1 + 216 | 0;
     $4 = HEAP32[$3 + 4 >> 2];
     $7 = HEAP32[$3 >> 2];
     if (($4 | 0) < 0) {
      $8 = 1
     } else {
      if (($4 | 0) <= 0) {
       $8 = $7 >>> 0 >= 1 ? 0 : 1
      } else {
       $8 = 0
      }
     }
     if ($8) {
      break label$2
     }
     HEAP32[$3 >> 2] = 0;
     HEAP32[$3 + 4 >> 2] = 0;
     break label$2;
    }
    HEAP32[$9 + 8 >> 2] = 23229;
    HEAP32[$9 + 4 >> 2] = 1463;
    HEAP32[$9 >> 2] = 23092;
    printk(23061, $9);
    abort();
   }
   $7 = $2 & 2;
   if (!(HEAP32[$1 + 20 >> 2] != 2 ? !$7 : 0)) {
    $3 = $0 + 1008 | 0;
    $5 = $3;
    $6 = $1 + 200 | 0;
    $10 = HEAP32[$6 >> 2];
    $4 = $10 + HEAP32[$3 >> 2] | 0;
    $6 = HEAP32[$6 + 4 >> 2];
    $3 = $6 + HEAP32[$3 + 4 >> 2] | 0;
    HEAP32[$5 >> 2] = $4;
    HEAP32[$5 + 4 >> 2] = $4 >>> 0 < $10 >>> 0 ? $3 + 1 | 0 : $3;
    $0 = $0 + 1e3 | 0;
    $4 = $0;
    $8 = $0;
    $3 = HEAP32[$0 + 4 >> 2] + $6 | 0;
    $0 = HEAP32[$0 >> 2];
    $5 = $0 + $10 | 0;
    if ($5 >>> 0 < $0 >>> 0) {
     $3 = $3 + 1 | 0
    }
    HEAP32[$8 >> 2] = $5;
    HEAP32[$4 + 4 >> 2] = $3;
   }
   $0 = HEAPU8[$1 + 236 | 0];
   $3 = $0 & 1;
   label$7 : {
    label$8 : {
     label$9 : {
      label$10 : {
       label$11 : {
        label$12 : {
         label$13 : {
          $4 = $2 & 32;
          if ($4) {
           break label$13
          }
          if (!$3) {
           break label$13
          }
          if (!($2 & 1)) {
           break label$1
          }
          $2 = $1 + 176 | 0;
          if (!(HEAP32[$2 >> 2] | HEAP32[$2 + 4 >> 2])) {
           break label$1
          }
          if ($0 & 8) {
           break label$12
          }
          $1 = $1 + 200 | 0;
          $2 = HEAP32[$1 >> 2];
          $0 = $2 + HEAP32[3302] | 0;
          $6 = HEAP32[3303] + HEAP32[$1 + 4 >> 2] | 0;
          HEAP32[3302] = $0;
          HEAP32[3303] = $0 >>> 0 < $2 >>> 0 ? $6 + 1 | 0 : $6;
          break label$1;
         }
         $13 = HEAP32[$11 >> 2];
         if (($13 | 0) != ($11 | 0)) {
          break label$8
         }
         label$14 : {
          if (!($2 & 1)) {
           if ($4) {
            break label$14
           }
           if (!$7) {
            break label$9
           }
           $0 = $1 + 224 | 0;
           $2 = HEAP32[$0 >> 2];
           $8 = HEAP32[$0 + 4 >> 2];
           $7 = HEAP32[3319];
           $0 = HEAP32[3318];
           $4 = $8 - ($7 + ($2 >>> 0 < $0 >>> 0) | 0) | 0;
           $2 = $2 - $0 | 0;
           if (($4 | 0) > -1) {
            $2 = 1
           } else {
            if (($4 | 0) >= -1) {
             $2 = $2 >>> 0 <= 4294967295 ? 0 : 1
            } else {
             $2 = 0
            }
           }
           if ($2) {
            break label$9
           }
           if ($3) {
            break label$9
           }
           $3 = $1 + 176 | 0;
           $4 = HEAP32[$3 + 4 >> 2];
           $2 = $1 + 216 | 0;
           HEAP32[$2 >> 2] = HEAP32[$3 >> 2];
           HEAP32[$2 + 4 >> 2] = $4;
           $3 = $1 + 184 | 0;
           $2 = $0 + HEAP32[$3 >> 2] | 0;
           $3 = $7 + HEAP32[$3 + 4 >> 2] | 0;
           $4 = $1 + 224 | 0;
           HEAP32[$4 >> 2] = $2;
           HEAP32[$4 + 4 >> 2] = $2 >>> 0 < $0 >>> 0 ? $3 + 1 | 0 : $3;
           break label$9;
          }
          $2 = $1 + 176 | 0;
          if (!(HEAP32[$2 >> 2] | HEAP32[$2 + 4 >> 2])) {
           break label$10
          }
          if ($0 & 8) {
           break label$11
          }
          $0 = $1 + 200 | 0;
          $3 = HEAP32[$0 >> 2];
          $2 = $3 + HEAP32[3302] | 0;
          $0 = HEAP32[3303] + HEAP32[$0 + 4 >> 2] | 0;
          HEAP32[3302] = $2;
          HEAP32[3303] = $2 >>> 0 < $3 >>> 0 ? $0 + 1 | 0 : $0;
          break label$10;
         }
         replenish_dl_entity($11, $11);
         break label$9;
        }
        HEAP8[$1 + 236 | 0] = $0 & 247;
        if ((hrtimer_try_to_cancel($1 + 288 | 0) | 0) != 1) {
         break label$1
        }
        $1 = $11 + -152 | 0;
        $0 = HEAP32[$1 >> 2] + -1 | 0;
        HEAP32[$1 >> 2] = $0;
        if ($0) {
         break label$1
        }
        __put_task_struct($11 + -160 | 0);
        break label$1;
       }
       HEAP8[$1 + 236 | 0] = $0 & 247;
       if ((hrtimer_try_to_cancel($1 + 288 | 0) | 0) != 1) {
        break label$10
       }
       $2 = $11 + -152 | 0;
       $0 = HEAP32[$2 >> 2] + -1 | 0;
       HEAP32[$2 >> 2] = $0;
       if ($0) {
        break label$10
       }
       __put_task_struct($11 + -160 | 0);
      }
      $0 = $1 + 184 | 0;
      $10 = HEAP32[$0 >> 2];
      $2 = HEAP32[$0 + 4 >> 2];
      $0 = $1 + 224 | 0;
      $3 = HEAP32[$0 >> 2];
      $14 = HEAP32[3319];
      $7 = HEAP32[3318];
      $4 = $7;
      $0 = HEAP32[$0 + 4 >> 2] - ($14 + ($3 >>> 0 < $4 >>> 0) | 0) | 0;
      $8 = $3 - $4 | 0;
      $3 = $8;
      if (($0 | 0) < -1) {
       $3 = 1
      } else {
       if (($0 | 0) <= -1) {
        $3 = $3 >>> 0 > 4294967295 ? 0 : 1
       } else {
        $3 = 0
       }
      }
      label$16 : {
       if (!$3) {
        $4 = $1 + 176 | 0;
        $3 = HEAP32[$4 + 4 >> 2];
        $4 = HEAP32[$4 >> 2];
        $5 = __wasm_i64_mul(($3 & 1023) << 22 | $4 >>> 10, $3 >>> 10 | 0, ($0 & 1023) << 22 | $8 >>> 10, $0 >>> 10 | 0);
        $15 = i64toi32_i32$HIGH_BITS;
        $6 = $1 + 216 | 0;
        $12 = HEAP32[$6 + 4 >> 2];
        $6 = HEAP32[$6 >> 2];
        $6 = __wasm_i64_mul(($12 & 1023) << 22 | $6 >>> 10, $12 >> 10, ($2 & 1023) << 22 | $10 >>> 10, $2 >>> 10 | 0);
        $12 = $15 - (i64toi32_i32$HIGH_BITS + ($5 >>> 0 < $6 >>> 0) | 0) | 0;
        $5 = $5 - $6 | 0;
        if (($12 | 0) > -1) {
         $5 = 1
        } else {
         if (($12 | 0) >= -1) {
          $5 = $5 >>> 0 <= 4294967295 ? 0 : 1
         } else {
          $5 = 0
         }
        }
        if ($5) {
         break label$9
        }
        $5 = $1 + 192 | 0;
        if (HEAP32[$5 >> 2] == ($10 | 0) & HEAP32[$5 + 4 >> 2] == ($2 | 0)) {
         break label$16
        }
        if (HEAPU8[$1 + 236 | 0] & 2) {
         break label$16
        }
        $3 = $1 + 208 | 0;
        $3 = __wasm_i64_mul(HEAP32[$3 >> 2], HEAP32[$3 + 4 >> 2], $8, $0);
        $0 = i64toi32_i32$HIGH_BITS;
        $4 = $0 >>> 20 | 0;
        $2 = $1 + 216 | 0;
        HEAP32[$2 >> 2] = ($0 & 1048575) << 12 | $3 >>> 20;
        HEAP32[$2 + 4 >> 2] = $4;
        break label$9;
       }
       $0 = $1 + 176 | 0;
       $4 = HEAP32[$0 >> 2];
       $3 = HEAP32[$0 + 4 >> 2];
      }
      $0 = $1 + 216 | 0;
      HEAP32[$0 >> 2] = $4;
      HEAP32[$0 + 4 >> 2] = $3;
      $0 = $2 + $14 | 0;
      $2 = $7 + $10 | 0;
      if ($2 >>> 0 < $7 >>> 0) {
       $0 = $0 + 1 | 0
      }
      $3 = $1 + 224 | 0;
      HEAP32[$3 >> 2] = $2;
      HEAP32[$3 + 4 >> 2] = $0;
     }
     if (HEAP32[$11 >> 2] != ($13 | 0)) {
      break label$7
     }
     $7 = 1;
     $2 = 0;
     $3 = 13176;
     $0 = HEAP32[3294];
     if ($0) {
      $2 = $1 + 224 | 0;
      $4 = HEAP32[$2 >> 2];
      $5 = HEAP32[$2 + 4 >> 2];
      while (1) {
       $2 = $0;
       $0 = $0 - -64 | 0;
       $3 = HEAP32[$0 >> 2];
       $0 = $5 - (HEAP32[$0 + 4 >> 2] + ($4 >>> 0 < $3 >>> 0) | 0) | 0;
       $3 = $4 - $3 | 0;
       $7 = $0 >> 31 & $7;
       if (($0 | 0) < 0) {
        $3 = 1
       } else {
        if (($0 | 0) <= 0) {
         $3 = $3 >>> 0 >= 0 ? 0 : 1
        } else {
         $3 = 0
        }
       }
       $0 = HEAP32[($3 ? 8 : 4) + $2 >> 2];
       if ($0) {
        continue
       }
       break;
      };
      $7 = ($7 | 0) != 0;
      $3 = $3 ? $2 + 8 | 0 : $2 + 4 | 0;
     }
     $0 = $1 + 164 | 0;
     HEAP32[$0 >> 2] = 0;
     HEAP32[$0 + 4 >> 2] = 0;
     HEAP32[$1 + 160 >> 2] = $2;
     HEAP32[$3 >> 2] = $11;
     rb_insert_color_cached($11, 13176, $7);
     HEAP32[3296] = HEAP32[3296] + 1;
     HEAP32[3052] = HEAP32[3052] + 1;
     global$0 = $9 + 48 | 0;
     return;
    }
    HEAP32[$9 + 40 >> 2] = 23245;
    HEAP32[$9 + 36 >> 2] = 1414;
    HEAP32[$9 + 32 >> 2] = 23092;
    printk(23061, $9 + 32 | 0);
    abort();
   }
   HEAP32[$9 + 24 >> 2] = 23263;
   HEAP32[$9 + 20 >> 2] = 1378;
   HEAP32[$9 + 16 >> 2] = 23092;
   printk(23061, $9 + 16 | 0);
   abort();
  }
  global$0 = $9 + 48 | 0;
 }
 
 function replenish_dl_entity($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $8 = HEAP32[$1 + 16 >> 2];
  $9 = HEAP32[$1 + 20 >> 2];
  label$1 : {
   label$2 : {
    if ($8 | $9) {
     if (!(HEAP32[$0 + 24 >> 2] != 0 | HEAP32[$0 + 28 >> 2] != 0)) {
      HEAP32[$0 + 56 >> 2] = $8;
      HEAP32[$0 + 60 >> 2] = $9;
      $2 = HEAP32[$1 + 28 >> 2] + HEAP32[3319] | 0;
      $3 = HEAP32[3318];
      $4 = $3 + HEAP32[$1 + 24 >> 2] | 0;
      if ($4 >>> 0 < $3 >>> 0) {
       $2 = $2 + 1 | 0
      }
      HEAP32[$0 + 64 >> 2] = $4;
      HEAP32[$0 + 68 >> 2] = $2;
     }
     $3 = HEAP32[$0 + 56 >> 2];
     $2 = HEAP32[$0 + 60 >> 2];
     $10 = HEAPU8[$0 + 76 | 0];
     $12 = $10 & 4;
     label$5 : {
      label$6 : {
       if (!$12) {
        if (($2 | 0) < 0) {
         $5 = 1
        } else {
         if (($2 | 0) <= 0) {
          $5 = $3 >>> 0 >= 1 ? 0 : 1
         } else {
          $5 = 0
         }
        }
        if ($5) {
         break label$6
        }
        $4 = HEAP32[$0 + 64 >> 2];
        $6 = HEAP32[$0 + 68 >> 2];
        break label$5;
       }
       if (($2 | 0) < 0) {
        $5 = 1
       } else {
        if (($2 | 0) <= 0) {
         $5 = $3 >>> 0 >= 1 ? 0 : 1
        } else {
         $5 = 0
        }
       }
       if ($5) {
        break label$6
       }
       $3 = 0;
       $2 = 0;
       $4 = $0 + 56 | 0;
       HEAP32[$4 >> 2] = 0;
       HEAP32[$4 + 4 >> 2] = 0;
      }
      $4 = HEAP32[$0 + 64 >> 2];
      $6 = HEAP32[$0 + 68 >> 2];
      $5 = HEAP32[$1 + 32 >> 2];
      $13 = HEAP32[$1 + 36 >> 2];
      while (1) {
       $6 = $6 + $13 | 0;
       $4 = $4 + $5 | 0;
       if ($4 >>> 0 < $5 >>> 0) {
        $6 = $6 + 1 | 0
       }
       $2 = $2 + $9 | 0;
       $3 = $3 + $8 | 0;
       if ($3 >>> 0 < $8 >>> 0) {
        $2 = $2 + 1 | 0
       }
       $11 = $3;
       if (($2 | 0) < 0) {
        $11 = 1
       } else {
        if (($2 | 0) <= 0) {
         $11 = $11 >>> 0 >= 1 ? 0 : 1
        } else {
         $11 = 0
        }
       }
       if ($11) {
        continue
       }
       break;
      };
      $5 = $0 + 56 | 0;
      HEAP32[$5 >> 2] = $3;
      HEAP32[$5 + 4 >> 2] = $2;
      $3 = $0 - -64 | 0;
      HEAP32[$3 >> 2] = $4;
      HEAP32[$3 + 4 >> 2] = $6;
     }
     $2 = HEAP32[3319];
     $3 = HEAP32[3318];
     $6 = $6 - ($2 + ($4 >>> 0 < $3 >>> 0) | 0) | 0;
     $4 = $4 - $3 | 0;
     if (($6 | 0) > -1) {
      $5 = 1
     } else {
      if (($6 | 0) >= -1) {
       $5 = $4 >>> 0 <= 4294967295 ? 0 : 1
      } else {
       $5 = 0
      }
     }
     if ($5) {
      break label$1
     }
     if (HEAPU8[79432]) {
      break label$2
     }
     HEAP8[79432] = 1;
     printk_deferred(23192, 0);
     $10 = HEAPU8[$0 + 76 | 0];
     $12 = $10 & 4;
     $3 = $1 + 16 | 0;
     $8 = HEAP32[$3 >> 2];
     $9 = HEAP32[$3 + 4 >> 2];
     $3 = HEAP32[3318];
     $2 = HEAP32[3319];
     break label$2;
    }
    HEAP32[$7 + 8 >> 2] = 23172;
    HEAP32[$7 + 4 >> 2] = 672;
    HEAP32[$7 >> 2] = 23092;
    printk(23061, $7);
    abort();
   }
   $4 = $0 + 56 | 0;
   HEAP32[$4 >> 2] = $8;
   HEAP32[$4 + 4 >> 2] = $9;
   $2 = $2 + HEAP32[$1 + 28 >> 2] | 0;
   $1 = $3 + HEAP32[$1 + 24 >> 2] | 0;
   if ($1 >>> 0 < $3 >>> 0) {
    $2 = $2 + 1 | 0
   }
   $4 = $0 - -64 | 0;
   HEAP32[$4 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = $2;
  }
  $1 = $12 ? $10 & 251 : $10;
  if ($1 & 1 | $12) {
   HEAP8[$0 + 76 | 0] = $1 & 254
  }
  global$0 = $7 + 16 | 0;
 }
 
 function inactive_task_timer($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $6 = $0 + -288 | 0;
  $1 = task_rq_lock($6, $7 + 8 | 0);
  update_rq_clock($1);
  $2 = $0 + -128 | 0;
  label$1 : {
   label$2 : {
    if (!(HEAP32[$6 >> 2] != 128 ? HEAP32[$0 + -264 >> 2] <= -1 : 0)) {
     label$5 : {
      if (HEAP32[$6 >> 2] != 128) {
       break label$5
      }
      if (!(HEAPU8[$2 + 76 | 0] & 8)) {
       break label$5
      }
      $3 = HEAP32[3303];
      $1 = HEAP32[$2 + 44 >> 2];
      $4 = HEAP32[3302];
      $0 = HEAP32[$2 + 40 >> 2];
      $5 = ($3 | 0) == ($1 | 0) & $4 >>> 0 < $0 >>> 0 | $3 >>> 0 < $1 >>> 0;
      $4 = $5 ? $0 : $4;
      HEAP32[3302] = $4 - $0;
      HEAP32[3303] = ($5 ? $1 : $3) - (($4 >>> 0 < $0 >>> 0) + $1 | 0);
      $3 = HEAP32[3305];
      $4 = HEAP32[3304];
      $5 = ($3 | 0) == ($1 | 0) & $4 >>> 0 < $0 >>> 0 | $3 >>> 0 < $1 >>> 0;
      $4 = $5 ? $0 : $4;
      HEAP32[3304] = $4 - $0;
      HEAP32[3305] = ($5 ? $1 : $3) - (($4 >>> 0 < $0 >>> 0) + $1 | 0);
      $0 = $2 + 76 | 0;
      HEAP8[$0 | 0] = HEAPU8[$0 | 0] & 247;
     }
     $1 = HEAP32[3300];
     $3 = HEAP32[$2 + 40 >> 2];
     $4 = HEAP32[3301] - (HEAP32[$2 + 44 >> 2] + ($1 >>> 0 < $3 >>> 0) | 0) | 0;
     HEAP32[3300] = $1 - $3;
     HEAP32[3301] = $4;
     $0 = $3;
     $3 = $0 >> 31;
     $4 = $0;
     $1 = $0 + HEAP32[3306] | 0;
     $0 = $3 + HEAP32[3307] | 0;
     HEAP32[3306] = $1;
     HEAP32[3307] = $1 >>> 0 < $4 >>> 0 ? $0 + 1 | 0 : $0;
     HEAP32[$2 + 72 >> 2] = 0;
     HEAP32[$2 + 32 >> 2] = 0;
     HEAP32[$2 + 36 >> 2] = 0;
     HEAP32[$2 + 24 >> 2] = 0;
     HEAP32[$2 + 28 >> 2] = 0;
     HEAP32[$2 + 16 >> 2] = 0;
     HEAP32[$2 + 20 >> 2] = 0;
     HEAP32[$2 + 48 >> 2] = 0;
     HEAP32[$2 + 52 >> 2] = 0;
     HEAP32[$2 + 40 >> 2] = 0;
     HEAP32[$2 + 44 >> 2] = 0;
     $1 = 226;
     $0 = $6 + 236 | 0;
     break label$2;
    }
    if (!(HEAPU8[$2 + 76 | 0] & 8)) {
     break label$1
    }
    $5 = $1 + 1e3 | 0;
    $3 = $5;
    $4 = HEAP32[$3 >> 2];
    $1 = HEAP32[$2 + 40 >> 2];
    $8 = $4;
    $0 = HEAP32[$3 + 4 >> 2];
    $3 = HEAP32[$2 + 44 >> 2];
    $4 = ($0 | 0) == ($3 | 0) & $4 >>> 0 < $1 >>> 0 | $0 >>> 0 < $3 >>> 0;
    $8 = $4 ? $1 : $8;
    HEAP32[$5 >> 2] = $8 - $1;
    HEAP32[$5 + 4 >> 2] = ($4 ? $3 : $0) - (($8 >>> 0 < $1 >>> 0) + $3 | 0);
    $1 = 247;
    $0 = $2 + 76 | 0;
   }
   HEAP8[$0 | 0] = HEAPU8[$0 | 0] & $1;
  }
  HEAP32[19859] = HEAP32[$7 + 8 >> 2];
  $1 = $2 + -152 | 0;
  $0 = HEAP32[$1 >> 2] + -1 | 0;
  HEAP32[$1 >> 2] = $0;
  if (!$0) {
   __put_task_struct($6)
  }
  global$0 = $7 + 16 | 0;
  return 0;
 }
 
 function start_dl_timer($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $5 = $0 + 184 | 0;
  $1 = HEAP32[$5 >> 2];
  $3 = HEAP32[$5 + 4 >> 2];
  $2 = $0 + 224 | 0;
  $5 = HEAP32[$2 >> 2];
  $9 = HEAP32[$2 + 4 >> 2];
  $2 = $0 + 192 | 0;
  $7 = HEAP32[$2 >> 2];
  $8 = HEAP32[$2 + 4 >> 2];
  $12 = FUNCTION_TABLE[HEAP32[HEAP32[$0 + 276 >> 2] + 28 >> 2]]() | 0;
  $14 = i64toi32_i32$HIGH_BITS;
  $2 = $7;
  $7 = $5 - $1 | 0;
  $2 = $2 + $7 | 0;
  $1 = ($9 - (($5 >>> 0 < $1 >>> 0) + $3 | 0) | 0) + $8 | 0;
  $9 = $2 >>> 0 < $7 >>> 0 ? $1 + 1 | 0 : $1;
  $13 = HEAP32[3319];
  $7 = $2;
  $8 = HEAP32[3318];
  $1 = $8;
  $5 = $9 - ($13 + ($2 >>> 0 < $1 >>> 0) | 0) | 0;
  $2 = $2 - $1 | 0;
  $15 = $2;
  $1 = $5;
  $6 = $1 >> 31;
  $3 = $1 >> 31;
  $1 = $1 + $6 | 0;
  $2 = $2 + $3 | 0;
  if ($2 >>> 0 < $3 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $2 = $2 ^ $3;
  $3 = $1 ^ $6;
  $6 = $2;
  $4 = __wasm_i64_mul($2, 0, -1924145349, 0);
  $1 = i64toi32_i32$HIGH_BITS;
  $10 = $1;
  $1 = $1 + -2095944041 | 0;
  $2 = $4 + -1924145349 | 0;
  if ($2 >>> 0 < 2370821947) {
   $1 = $1 + 1 | 0
  }
  $11 = $2;
  $2 = $1;
  $1 = ($10 | 0) == ($1 | 0) & $11 >>> 0 < $4 >>> 0 | $1 >>> 0 < $10 >>> 0;
  $10 = $3;
  $4 = 0;
  $3 = __wasm_i64_mul($3, $4, -1924145349, 0);
  $2 = $3 + $2 | 0;
  $1 = i64toi32_i32$HIGH_BITS + $1 | 0;
  $1 = $2 >>> 0 < $3 >>> 0 ? $1 + 1 | 0 : $1;
  $4 = $2;
  $6 = __wasm_i64_mul($6, 0, -2095944041, 0);
  $2 = $2 + $6 | 0;
  $3 = $1;
  $1 = $1 + i64toi32_i32$HIGH_BITS | 0;
  $1 = $2 >>> 0 < $6 >>> 0 ? $1 + 1 | 0 : $1;
  $11 = $2;
  $2 = $1;
  $1 = ($3 | 0) == ($1 | 0) & $11 >>> 0 < $4 >>> 0 | $1 >>> 0 < $3 >>> 0;
  $6 = __wasm_i64_mul($10, 0, -2095944041, 0);
  $2 = $6 + $2 | 0;
  $4 = i64toi32_i32$HIGH_BITS + $1 | 0;
  $1 = $2;
  $4 = $1 >>> 0 < $6 >>> 0 ? $4 + 1 | 0 : $4;
  $1 = ($4 & 511) << 23 | $1 >>> 9;
  $2 = 0 - $1 & $15;
  $1 = 0 - (($4 >>> 9 | 0) + (0 < $1 >>> 0) | 0) & $5;
  if (($1 | 0) < 0) {
   $2 = 1
  } else {
   if (($1 | 0) <= 0) {
    $2 = $2 >>> 0 >= 0 ? 0 : 1
   } else {
    $2 = 0
   }
  }
  label$1 : {
   if ($2) {
    break label$1
   }
   $16 = 1;
   if (HEAP8[$0 + 280 | 0] & 1) {
    break label$1
   }
   HEAP32[$0 + 8 >> 2] = HEAP32[$0 + 8 >> 2] + 1;
   $2 = $0 + 240 | 0;
   $1 = ($14 - (($12 >>> 0 < $8 >>> 0) + $13 | 0) | 0) + $9 | 0;
   $5 = $12 - $8 | 0;
   $0 = $5 + $7 | 0;
   if ($0 >>> 0 < $5 >>> 0) {
    $1 = $1 + 1 | 0
   }
   hrtimer_start_range_ns($2, $0, $1, 0, 0, 0);
  }
  return $16;
 }
 
 function dequeue_task_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  update_curr_dl($0);
  $3 = $1 + 160 | 0;
  if (($3 | 0) != HEAP32[$1 + 160 >> 2]) {
   rb_erase_cached($3, 13176);
   HEAP32[3296] = HEAP32[3296] + -1;
   HEAP32[3052] = HEAP32[3052] + -1;
   HEAP32[$3 >> 2] = $3;
  }
  if (!(HEAP32[$1 + 20 >> 2] != 2 ? !($2 & 2) : 0)) {
   $8 = $0 + 1e3 | 0;
   $4 = $8;
   $6 = HEAP32[$4 >> 2];
   $5 = $1 + 200 | 0;
   $3 = HEAP32[$5 >> 2];
   $9 = $6;
   $7 = HEAP32[$4 + 4 >> 2];
   $5 = HEAP32[$5 + 4 >> 2];
   $6 = ($7 | 0) == ($5 | 0) & $6 >>> 0 < $3 >>> 0 | $7 >>> 0 < $5 >>> 0;
   $4 = $6 ? $3 : $9;
   HEAP32[$8 >> 2] = $4 - $3;
   HEAP32[$8 + 4 >> 2] = ($6 ? $5 : $7) - (($4 >>> 0 < $3 >>> 0) + $5 | 0);
   $7 = $0 + 1008 | 0;
   $0 = $7;
   $4 = HEAP32[$0 >> 2];
   $9 = $4;
   $6 = HEAP32[$0 + 4 >> 2];
   $4 = ($6 | 0) == ($5 | 0) & $4 >>> 0 < $3 >>> 0 | $6 >>> 0 < $5 >>> 0;
   $0 = $4 ? $3 : $9;
   HEAP32[$7 >> 2] = $0 - $3;
   HEAP32[$7 + 4 >> 2] = ($4 ? $5 : $6) - (($0 >>> 0 < $3 >>> 0) + $5 | 0);
  }
  if ($2 & 1) {
   task_non_contending($1)
  }
 }
 
 function update_curr_dl($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  label$1 : {
   $4 = HEAP32[$0 + 1036 >> 2];
   if (HEAP32[$4 + 24 >> 2] > -1) {
    break label$1
   }
   $10 = $4 + 160 | 0;
   if (($10 | 0) == HEAP32[$10 >> 2]) {
    break label$1
   }
   label$2 : {
    label$3 : {
     label$4 : {
      $1 = $4 + 88 | 0;
      $6 = HEAP32[$1 >> 2];
      $3 = HEAP32[$0 + 1076 >> 2];
      $2 = HEAP32[$0 + 1072 >> 2];
      $8 = $3 - (HEAP32[$1 + 4 >> 2] + ($2 >>> 0 < $6 >>> 0) | 0) | 0;
      $6 = $2 - $6 | 0;
      $5 = $6;
      if (($8 | 0) < 0) {
       $5 = 1
      } else {
       if (($8 | 0) <= 0) {
        $5 = $5 >>> 0 > 0 ? 0 : 1
       } else {
        $5 = 0
       }
      }
      if (!$5) {
       HEAP32[$1 >> 2] = $2;
       HEAP32[$1 + 4 >> 2] = $3;
       $3 = $4 + 96 | 0;
       $2 = $3;
       $5 = $2;
       $1 = $8 + HEAP32[$2 + 4 >> 2] | 0;
       $3 = $6 + HEAP32[$2 >> 2] | 0;
       if ($3 >>> 0 < $6 >>> 0) {
        $1 = $1 + 1 | 0
       }
       HEAP32[$5 >> 2] = $3;
       HEAP32[$2 + 4 >> 2] = $1;
       if (HEAPU8[$4 + 232 | 0] & 2) {
        break label$4
       }
       $1 = $8 & 4194303;
       $3 = $6;
       break label$3;
      }
      $2 = HEAPU8[$4 + 236 | 0];
      if (!($2 & 4)) {
       break label$1
      }
      $1 = $4 + 216 | 0;
      $3 = HEAP32[$1 >> 2];
      $1 = HEAP32[$1 + 4 >> 2];
      break label$2;
     }
     $1 = $0 + 1024 | 0;
     $2 = HEAP32[$1 >> 2];
     $5 = HEAP32[$1 + 4 >> 2];
     $1 = $4 + 200 | 0;
     $1 = __wasm_i64_mul($2, $5, HEAP32[$1 >> 2], HEAP32[$1 + 4 >> 2]);
     $2 = i64toi32_i32$HIGH_BITS;
     $5 = $2 >>> 8 | 0;
     $16 = $5;
     $7 = $0 + 1008 | 0;
     $9 = HEAP32[$7 >> 2];
     $11 = $0 + 1e3 | 0;
     $12 = HEAP32[$11 >> 2];
     $3 = $9 - $12 | 0;
     $13 = 1048576 - $3 | 0;
     $15 = $0 + 1016 | 0;
     $14 = HEAP32[$15 >> 2];
     $7 = HEAP32[$7 + 4 >> 2] - (HEAP32[$11 + 4 >> 2] + ($9 >>> 0 < $12 >>> 0) | 0) | 0;
     $9 = HEAP32[$15 + 4 >> 2];
     $11 = 0 - ($7 + (1048576 < $3 >>> 0) | 0) - ($9 + ($13 >>> 0 < $14 >>> 0)) | 0;
     $2 = ($2 & 255) << 24 | $1 >>> 8;
     $12 = $2;
     $13 = $13 - $14 | 0;
     $1 = $7 + $9 | 0;
     $7 = $3 + $14 | 0;
     if ($7 >>> 0 < $3 >>> 0) {
      $1 = $1 + 1 | 0
     }
     $3 = 0 - ((1048576 < $2 >>> 0) + $5 | 0) | 0;
     $1 = ($3 | 0) == ($1 | 0) & $7 >>> 0 > 1048576 - $2 >>> 0 | $1 >>> 0 > $3 >>> 0;
     $3 = __wasm_i64_mul($1 ? $12 : $13, $1 ? $16 : $11, $6, $8);
     $2 = i64toi32_i32$HIGH_BITS;
     $1 = $2 >>> 20 | 0;
     $3 = ($2 & 1048575) << 12 | $3 >>> 20;
    }
    $2 = $4 + 216 | 0;
    $5 = HEAP32[$2 >> 2];
    $1 = HEAP32[$2 + 4 >> 2] - (($5 >>> 0 < $3 >>> 0) + $1 | 0) | 0;
    $3 = $5 - $3 | 0;
    HEAP32[$2 >> 2] = $3;
    HEAP32[$2 + 4 >> 2] = $1;
    $2 = HEAPU8[$4 + 236 | 0];
   }
   if (($1 | 0) < 0) {
    $1 = 1
   } else {
    if (($1 | 0) <= 0) {
     $1 = $3 >>> 0 >= 1 ? 0 : 1
    } else {
     $1 = 0
    }
   }
   label$6 : {
    label$7 : {
     label$8 : {
      if (!$1) {
       $3 = 1;
       if ($2 & 4) {
        break label$8
       }
       break label$6;
      }
      HEAP8[$4 + 236 | 0] = $2 | 1;
      $3 = 17;
      if (!(HEAPU8[$4 + 232 | 0] & 4)) {
       break label$7
      }
     }
     HEAP8[$4 + 236 | 0] = $3 | $2;
    }
    rb_erase_cached($10, 13176);
    HEAP32[3296] = HEAP32[3296] + -1;
    HEAP32[3052] = HEAP32[3052] + -1;
    HEAP32[$4 + 160 >> 2] = $10;
    label$10 : {
     if (!(HEAPU8[$4 + 236 | 0] & 2)) {
      if (start_dl_timer($4)) {
       break label$10
      }
     }
     enqueue_task_dl($0, $4, 32);
    }
    if (HEAP32[$0 + 972 >> 2] == ($4 + 160 | 0)) {
     break label$6
    }
    $4 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
    $1 = HEAP32[$4 >> 2];
    if (!($1 & 8)) {
     HEAP32[$4 >> 2] = $1 | 8
    }
   }
   if (HEAP32[3051] < 0) {
    break label$1
   }
   if (sched_rt_bandwidth_account($0 + 120 | 0)) {
    $4 = $0 + 952 | 0;
    $1 = $4;
    $2 = $1;
    $0 = $8 + HEAP32[$1 + 4 >> 2] | 0;
    $4 = $6 + HEAP32[$1 >> 2] | 0;
    if ($4 >>> 0 < $6 >>> 0) {
     $0 = $0 + 1 | 0
    }
    HEAP32[$2 >> 2] = $4;
    HEAP32[$1 + 4 >> 2] = $0;
   }
  }
 }
 
 function task_non_contending($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $2 = $0 + 176 | 0;
  $1 = $2;
  label$1 : {
   if (!(HEAP32[$1 >> 2] | HEAP32[$1 + 4 >> 2])) {
    break label$1
   }
   $5 = $0 + 288 | 0;
   hrtimer_active($5);
   $3 = $0 + 224 | 0;
   $1 = HEAP32[$3 >> 2];
   $3 = HEAP32[$3 + 4 >> 2];
   $4 = $0 + 192 | 0;
   $6 = HEAP32[$4 >> 2];
   $8 = HEAP32[$4 + 4 >> 2];
   $4 = $0 + 216 | 0;
   $4 = div_s64_rem(__wasm_i64_mul($6, $8, HEAP32[$4 >> 2], HEAP32[$4 + 4 >> 2]), i64toi32_i32$HIGH_BITS, HEAP32[$2 >> 2], $7 + 12 | 0);
   $2 = $1 - $4 | 0;
   $1 = $3 - (i64toi32_i32$HIGH_BITS + ($1 >>> 0 < $4 >>> 0) | 0) | 0;
   $3 = HEAP32[3318];
   $1 = $1 - (HEAP32[3319] + ($2 >>> 0 < $3 >>> 0) | 0) | 0;
   $2 = $2 - $3 | 0;
   if (($1 | 0) < -1) {
    $3 = 1
   } else {
    if (($1 | 0) <= -1) {
     $3 = $2 >>> 0 > 4294967295 ? 0 : 1
    } else {
     $3 = 0
    }
   }
   if (!$3) {
    HEAP32[$0 + 8 >> 2] = HEAP32[$0 + 8 >> 2] + 1;
    $0 = $0 + 236 | 0;
    HEAP8[$0 | 0] = HEAPU8[$0 | 0] | 8;
    hrtimer_start_range_ns($5, $2, $1, 0, 0, 1);
    break label$1;
   }
   if (HEAP32[$0 + 24 >> 2] <= -1) {
    $2 = $0 + 200 | 0;
    $1 = HEAP32[$2 >> 2];
    $5 = HEAP32[3303];
    $2 = HEAP32[$2 + 4 >> 2];
    $4 = HEAP32[3302];
    $3 = ($5 | 0) == ($2 | 0) & $4 >>> 0 < $1 >>> 0 | $5 >>> 0 < $2 >>> 0;
    $4 = $3 ? $1 : $4;
    HEAP32[3302] = $4 - $1;
    HEAP32[3303] = ($3 ? $2 : $5) - (($4 >>> 0 < $1 >>> 0) + $2 | 0);
    if (HEAP32[$0 >> 2] != 128) {
     break label$1
    }
   }
   if (HEAP32[$0 >> 2] == 128) {
    $2 = $0 + 200 | 0;
    $1 = HEAP32[$2 >> 2];
    $5 = HEAP32[3305];
    $2 = HEAP32[$2 + 4 >> 2];
    $4 = HEAP32[3304];
    $3 = ($5 | 0) == ($2 | 0) & $4 >>> 0 < $1 >>> 0 | $5 >>> 0 < $2 >>> 0;
    $4 = $3 ? $1 : $4;
    HEAP32[3304] = $4 - $1;
    HEAP32[3305] = ($3 ? $2 : $5) - (($4 >>> 0 < $1 >>> 0) + $2 | 0);
   }
   $2 = $0 + 200 | 0;
   $1 = $2;
   $3 = HEAP32[$1 >> 2];
   $4 = HEAP32[3300];
   $6 = HEAP32[3301] - (HEAP32[$1 + 4 >> 2] + ($4 >>> 0 < $3 >>> 0) | 0) | 0;
   HEAP32[3300] = $4 - $3;
   HEAP32[3301] = $6;
   $1 = $3;
   $3 = $1 >> 31;
   $6 = $1;
   $5 = $1 + HEAP32[3306] | 0;
   $1 = $3 + HEAP32[3307] | 0;
   HEAP32[3306] = $5;
   HEAP32[3307] = $5 >>> 0 < $6 >>> 0 ? $1 + 1 | 0 : $1;
   HEAP32[$0 + 232 >> 2] = 0;
   $1 = $0 + 192 | 0;
   HEAP32[$1 >> 2] = 0;
   HEAP32[$1 + 4 >> 2] = 0;
   $1 = $0 + 184 | 0;
   HEAP32[$1 >> 2] = 0;
   HEAP32[$1 + 4 >> 2] = 0;
   $1 = $0 + 176 | 0;
   HEAP32[$1 >> 2] = 0;
   HEAP32[$1 + 4 >> 2] = 0;
   $1 = $0 + 208 | 0;
   HEAP32[$1 >> 2] = 0;
   HEAP32[$1 + 4 >> 2] = 0;
   HEAP32[$2 >> 2] = 0;
   HEAP32[$2 + 4 >> 2] = 0;
   $0 = $0 + 236 | 0;
   HEAP8[$0 | 0] = HEAPU8[$0 | 0] & 226;
  }
  global$0 = $7 + 16 | 0;
 }
 
 function yield_task_dl($0) {
  $0 = $0 | 0;
  var $1 = 0;
  $1 = HEAP32[$0 + 1036 >> 2] + 236 | 0;
  HEAP8[$1 | 0] = HEAPU8[$1 | 0] | 4;
  update_rq_clock($0);
  update_curr_dl($0);
  HEAP32[$0 + 1056 >> 2] = HEAP32[$0 + 1056 >> 2] | 1;
 }
 
 function check_preempt_curr_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0;
  $2 = $1 + 224 | 0;
  $1 = HEAP32[$2 >> 2];
  $4 = HEAP32[$2 + 4 >> 2];
  $2 = HEAP32[$0 + 1036 >> 2] + 224 | 0;
  $3 = HEAP32[$2 >> 2];
  $2 = $4 - (($1 >>> 0 < $3 >>> 0) + HEAP32[$2 + 4 >> 2] | 0) | 0;
  $1 = $1 - $3 | 0;
  if (($2 | 0) < -1) {
   $1 = 1
  } else {
   if (($2 | 0) <= -1) {
    $1 = $1 >>> 0 > 4294967295 ? 0 : 1
   } else {
    $1 = 0
   }
  }
  if (!$1) {
   return
  }
  $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
 }
 
 function pick_next_task_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  if (HEAP32[$1 + 40 >> 2] == 23104) {
   update_curr_dl($0)
  }
  label$2 : {
   label$3 : {
    if (HEAP32[$0 + 976 >> 2]) {
     FUNCTION_TABLE[HEAP32[HEAP32[$1 + 40 >> 2] + 28 >> 2]]($0, $1);
     $1 = HEAP32[$0 + 972 >> 2];
     if (!$1) {
      break label$3
     }
     $4 = HEAP32[$0 + 1076 >> 2];
     $3 = $1 + -72 | 0;
     HEAP32[$3 >> 2] = HEAP32[$0 + 1072 >> 2];
     HEAP32[$3 + 4 >> 2] = $4;
     $0 = $1 + -160 | 0;
     break label$2;
    }
    $0 = 0;
    break label$2;
   }
   HEAP32[$2 + 8 >> 2] = 23283;
   HEAP32[$2 + 4 >> 2] = 1750;
   HEAP32[$2 >> 2] = 23092;
   printk(23061, $2);
   abort();
  }
  global$0 = $2 + 16 | 0;
  return $0 | 0;
 }
 
 function put_prev_task_dl($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  update_curr_dl($0);
 }
 
 function task_tick_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  update_curr_dl($0);
 }
 
 function switched_from_dl($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2] == 1) {
    $3 = $1 + 176 | 0;
    if (!(HEAP32[$3 >> 2] | HEAP32[$3 + 4 >> 2])) {
     break label$1
    }
    task_non_contending($1);
    if (HEAP32[$1 + 20 >> 2] == 1) {
     break label$1
    }
   }
   $4 = $1 + 200 | 0;
   $3 = HEAP32[$4 >> 2];
   $4 = HEAP32[$4 + 4 >> 2];
   if (HEAPU8[$1 + 236 | 0] & 8) {
    $5 = $0 + 1e3 | 0;
    $2 = HEAP32[$5 >> 2];
    $6 = $2;
    $7 = HEAP32[$5 + 4 >> 2];
    $2 = ($4 | 0) == ($7 | 0) & $2 >>> 0 < $3 >>> 0 | $7 >>> 0 < $4 >>> 0;
    $6 = $2 ? $3 : $6;
    HEAP32[$5 >> 2] = $6 - $3;
    HEAP32[$5 + 4 >> 2] = ($2 ? $4 : $7) - (($6 >>> 0 < $3 >>> 0) + $4 | 0);
   }
   $0 = $0 + 1008 | 0;
   $2 = $0;
   $5 = HEAP32[$2 >> 2];
   $6 = $5;
   $2 = HEAP32[$2 + 4 >> 2];
   $5 = ($4 | 0) == ($2 | 0) & $5 >>> 0 < $3 >>> 0 | $2 >>> 0 < $4 >>> 0;
   $7 = $5 ? $3 : $6;
   HEAP32[$0 >> 2] = $7 - $3;
   HEAP32[$0 + 4 >> 2] = ($5 ? $4 : $2) - (($7 >>> 0 < $3 >>> 0) + $4 | 0);
  }
  $0 = $1 + 236 | 0;
  $1 = HEAPU8[$0 | 0];
  if ($1 & 8) {
   HEAP8[$0 | 0] = $1 & 247
  }
 }
 
 function switched_to_dl($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0, $4 = 0;
  label$1 : {
   if ((hrtimer_try_to_cancel($1 + 288 | 0) | 0) != 1) {
    break label$1
   }
   $2 = HEAP32[$1 + 8 >> 2] + -1 | 0;
   HEAP32[$1 + 8 >> 2] = $2;
   if ($2) {
    break label$1
   }
   __put_task_struct($1);
  }
  label$2 : {
   if (HEAP32[$1 + 20 >> 2] == 1) {
    $2 = HEAP32[$0 + 1036 >> 2];
    if (($1 | 0) != ($2 | 0)) {
     if (HEAP32[$2 + 24 >> 2] > -1) {
      break label$2
     }
     $3 = $1 + 224 | 0;
     $1 = HEAP32[$3 >> 2];
     $4 = HEAP32[$3 + 4 >> 2];
     $2 = $2 + 224 | 0;
     $3 = HEAP32[$2 >> 2];
     $2 = $4 - (($1 >>> 0 < $3 >>> 0) + HEAP32[$2 + 4 >> 2] | 0) | 0;
     $1 = $1 - $3 | 0;
     if (($2 | 0) < -1) {
      $1 = 1
     } else {
      if (($2 | 0) <= -1) {
       $1 = $1 >>> 0 > 4294967295 ? 0 : 1
      } else {
       $1 = 0
      }
     }
     if ($1) {
      break label$2
     }
    }
    return;
   }
   $0 = $0 + 1008 | 0;
   $2 = $0;
   $3 = $1 + 200 | 0;
   $4 = HEAP32[$3 >> 2];
   $1 = $4 + HEAP32[$2 >> 2] | 0;
   $0 = HEAP32[$2 + 4 >> 2] + HEAP32[$3 + 4 >> 2] | 0;
   HEAP32[$2 >> 2] = $1;
   HEAP32[$2 + 4 >> 2] = $1 >>> 0 < $4 >>> 0 ? $0 + 1 | 0 : $0;
   return;
  }
  $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
 }
 
 function prio_changed_dl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  label$1 : {
   if (HEAP32[$1 + 20 >> 2] == 1) {
    break label$1
   }
   if (HEAP32[$0 + 1036 >> 2] == ($1 | 0)) {
    break label$1
   }
   return;
  }
  $0 = HEAP32[HEAP32[$0 + 1036 >> 2] + 4 >> 2];
  $1 = HEAP32[$0 >> 2];
  if (!($1 & 8)) {
   HEAP32[$0 >> 2] = $1 | 8
  }
 }
 
 function __dl_clear_params($0) {
  var $1 = 0;
  HEAP32[$0 + 232 >> 2] = 0;
  $1 = $0 + 192 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 184 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 176 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 208 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $0 + 200 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $0 = $0 + 236 | 0;
  HEAP8[$0 | 0] = HEAPU8[$0 | 0] & 226;
 }
 
 function __wake_up($0, $1, $2) {
  __wake_up_common_lock($0, $1, $2);
 }
 
 function __wake_up_common_lock($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  $7 = 1;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  $4 = $5 + 24 | 0;
  $8 = $5 + 20 | 0;
  HEAP32[$4 >> 2] = $8;
  HEAP32[$5 + 16 >> 2] = 0;
  HEAP32[$5 + 8 >> 2] = 0;
  HEAP32[$5 + 12 >> 2] = 0;
  HEAP32[$5 + 20 >> 2] = $8;
  $10 = HEAPU8[79440];
  HEAP8[79440] = 0;
  label$1 : {
   label$2 : {
    if (!(HEAPU8[$5 + 8 | 0] & 4)) {
     $3 = HEAP32[$0 >> 2];
     if (($3 | 0) != ($0 | 0)) {
      break label$2
     }
     break label$1;
    }
    $3 = HEAP32[$8 >> 2];
    $6 = HEAP32[$4 >> 2];
    HEAP32[$3 + 4 >> 2] = $6;
    HEAP32[$6 >> 2] = $3;
    HEAP32[$8 >> 2] = 256;
    HEAP32[$4 >> 2] = 512;
    HEAP32[$5 + 8 >> 2] = 0;
    if (($0 | 0) == ($3 | 0)) {
     break label$1
    }
   }
   $6 = 0;
   label$4 : {
    while (1) {
     label$5 : {
      $4 = $3;
      $3 = HEAP32[$3 >> 2];
      $9 = $4 + -12 | 0;
      $11 = HEAP32[$9 >> 2];
      if (!($11 & 4)) {
       $4 = FUNCTION_TABLE[HEAP32[$4 + -4 >> 2]]($9, $1, 0, $2) | 0;
       if (($4 | 0) < 0) {
        break label$1
       }
       label$8 : {
        if (!($11 & 1)) {
         break label$8
        }
        if (!$4) {
         break label$8
        }
        $7 = $7 + -1 | 0;
        if (!$7) {
         break label$4
        }
       }
       if (($0 | 0) != ($3 | 0) ? ($6 | 0) >= 64 : 0) {
        break label$5
       }
       $6 = $6 + 1 | 0;
      }
      if (($0 | 0) != ($3 | 0)) {
       continue
      }
      break label$1;
     }
     break;
    };
    $4 = HEAP32[$3 + 4 >> 2];
    HEAP32[$3 + 4 >> 2] = $8;
    HEAP32[$5 + 20 >> 2] = $3;
    HEAP32[$5 + 8 >> 2] = 4;
    HEAP32[$4 >> 2] = $8;
    HEAP32[$5 + 24 >> 2] = $4;
    break label$1;
   }
   $7 = 0;
  }
  HEAP8[79440] = $10 & 1;
  if (HEAPU8[$5 + 8 | 0] & 4) {
   $10 = $5 + 20 | 0;
   $9 = $5 + 24 | 0;
   while (1) {
    $11 = HEAPU8[79440];
    HEAP8[79440] = 0;
    label$12 : {
     label$13 : {
      if (!(HEAPU8[$5 + 8 | 0] & 4)) {
       $3 = HEAP32[$0 >> 2];
       if (($3 | 0) != ($0 | 0)) {
        break label$13
       }
       break label$12;
      }
      $3 = HEAP32[$10 >> 2];
      $4 = HEAP32[$9 >> 2];
      HEAP32[$3 + 4 >> 2] = $4;
      HEAP32[$4 >> 2] = $3;
      HEAP32[$10 >> 2] = 256;
      HEAP32[$9 >> 2] = 512;
      HEAP32[$5 + 8 >> 2] = 0;
      if (($0 | 0) == ($3 | 0)) {
       break label$12
      }
     }
     $6 = 0;
     label$15 : {
      while (1) {
       label$16 : {
        $4 = $3;
        $3 = HEAP32[$3 >> 2];
        $12 = $4 + -12 | 0;
        $13 = HEAP32[$12 >> 2];
        if (!($13 & 4)) {
         $4 = FUNCTION_TABLE[HEAP32[$4 + -4 >> 2]]($12, $1, 0, $2) | 0;
         if (($4 | 0) < 0) {
          break label$12
         }
         label$19 : {
          if (!($13 & 1)) {
           break label$19
          }
          if (!$4) {
           break label$19
          }
          $7 = $7 + -1 | 0;
          if (!$7) {
           break label$15
          }
         }
         if (($0 | 0) != ($3 | 0) ? ($6 | 0) >= 64 : 0) {
          break label$16
         }
         $6 = $6 + 1 | 0;
        }
        if (($0 | 0) != ($3 | 0)) {
         continue
        }
        break label$12;
       }
       break;
      };
      $4 = HEAP32[$3 + 4 >> 2];
      HEAP32[$3 + 4 >> 2] = $8;
      HEAP32[$10 >> 2] = $3;
      HEAP32[$5 + 8 >> 2] = 4;
      HEAP32[$4 >> 2] = $8;
      HEAP32[$9 >> 2] = $4;
      break label$12;
     }
     $7 = 0;
    }
    HEAP8[79440] = $11 & 1;
    if (HEAPU8[$5 + 8 | 0] & 4) {
     continue
    }
    break;
   };
  }
  global$0 = $5 + 32 | 0;
 }
 
 function __wake_up_locked($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $3 = 1;
  $2 = HEAP32[$0 >> 2];
  label$1 : {
   if (($2 | 0) == ($0 | 0)) {
    break label$1
   }
   while (1) {
    $1 = $2;
    $2 = HEAP32[$1 >> 2];
    $4 = $1 + -12 | 0;
    $5 = HEAP32[$4 >> 2];
    label$3 : {
     if ($5 & 4) {
      break label$3
     }
     $1 = FUNCTION_TABLE[HEAP32[$1 + -4 >> 2]]($4, 3, 0, 0) | 0;
     if (($1 | 0) < 0) {
      break label$1
     }
     if (!($5 & 1)) {
      break label$3
     }
     if (!$1) {
      break label$3
     }
     $3 = $3 + -1 | 0;
     if (!$3) {
      break label$1
     }
    }
    if (($0 | 0) != ($2 | 0)) {
     continue
    }
    break;
   };
  }
 }
 
 function __wake_up_locked_key_bookmark($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  label$1 : {
   label$2 : {
    label$3 : {
     if (!(HEAPU8[$2 | 0] & 4 ? !!$2 : 0)) {
      $3 = HEAP32[$0 >> 2];
      if (($3 | 0) != ($0 | 0)) {
       break label$3
      }
      break label$2;
     }
     $5 = $2 + 16 | 0;
     $4 = HEAP32[$5 >> 2];
     $3 = HEAP32[$2 + 12 >> 2];
     HEAP32[$4 >> 2] = $3;
     HEAP32[$3 + 4 >> 2] = $4;
     HEAP32[$2 + 12 >> 2] = 256;
     HEAP32[$2 >> 2] = 0;
     HEAP32[$5 >> 2] = 512;
     if (($0 | 0) == ($3 | 0)) {
      break label$2
     }
    }
    $5 = 0;
    while (1) {
     $4 = $3;
     $3 = HEAP32[$3 >> 2];
     $6 = $4 + -12 | 0;
     $7 = HEAP32[$6 >> 2];
     label$7 : {
      if ($7 & 4) {
       break label$7
      }
      $4 = FUNCTION_TABLE[HEAP32[$4 + -4 >> 2]]($6, 3, 0, $1) | 0;
      if (($4 | 0) < 0) {
       break label$2
      }
      if ($4 ? $7 & 1 : 0) {
       break label$2
      }
      if (!$2) {
       break label$7
      }
      $4 = $5 + 1 | 0;
      if (($5 | 0) >= 64) {
       $5 = $4;
       if (($0 | 0) == ($3 | 0)) {
        break label$7
       }
       break label$1;
      }
      $5 = $4;
     }
     if (($0 | 0) != ($3 | 0)) {
      continue
     }
     break;
    };
   }
   return;
  }
  HEAP32[$2 >> 2] = 4;
  $0 = HEAP32[$3 + 4 >> 2];
  $1 = $2 + 12 | 0;
  HEAP32[$3 + 4 >> 2] = $1;
  HEAP32[$2 + 12 >> 2] = $3;
  HEAP32[$0 >> 2] = $1;
  HEAP32[$2 + 16 >> 2] = $0;
 }
 
 function prepare_to_wait($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -2;
  $3 = HEAPU8[79440];
  HEAP8[79440] = 0;
  $1 = $0 + 12 | 0;
  if (($1 | 0) == HEAP32[$0 + 12 >> 2]) {
   $2 = HEAP32[4128];
   HEAP32[$2 + 4 >> 2] = $1;
   HEAP32[$1 >> 2] = $2;
   HEAP32[4128] = $1;
   HEAP32[$0 + 16 >> 2] = 16512;
  }
  HEAP32[(global$0 - 16 | 0) + 12 >> 2] = 2;
  HEAP32[HEAP32[2] >> 2] = 2;
  HEAP8[79440] = $3 & 1;
 }
 
 function init_wait_entry($0) {
  var $1 = 0;
  HEAP32[$0 >> 2] = 0;
  HEAP32[$0 + 8 >> 2] = 76;
  $1 = $0 + 12 | 0;
  HEAP32[$0 + 12 >> 2] = $1;
  HEAP32[$0 + 4 >> 2] = HEAP32[2];
  HEAP32[$0 + 16 >> 2] = $1;
 }
 
 function autoremove_wake_function($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0;
  $1 = default_wake_function($0, $1, $2, $3);
  if ($1) {
   $2 = $0 + 16 | 0;
   $3 = HEAP32[$2 >> 2];
   $4 = HEAP32[$0 + 12 >> 2];
   HEAP32[$3 >> 2] = $4;
   HEAP32[$4 + 4 >> 2] = $3;
   $3 = $0;
   $0 = $0 + 12 | 0;
   HEAP32[$3 + 12 >> 2] = $0;
   HEAP32[$2 >> 2] = $0;
  }
  return $1 | 0;
 }
 
 function prepare_to_wait_event($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $7 = global$0 - 16 | 0;
  $8 = HEAPU8[79440];
  HEAP8[79440] = 0;
  label$1 : {
   label$2 : {
    label$3 : {
     if ($2 & 257) {
      $4 = HEAP32[2];
      if (HEAP32[HEAP32[$4 + 4 >> 2] >> 2] & 4) {
       break label$3
      }
     }
     $3 = $1 + 12 | 0;
     break label$2;
    }
    label$5 : {
     if (!($2 & 1)) {
      $3 = $1 + 12 | 0;
      if (!(HEAP8[$4 + 761 | 0] & 1)) {
       break label$2
      }
      break label$5;
     }
     $3 = $1 + 12 | 0;
    }
    $0 = HEAP32[$1 + 12 >> 2];
    $1 = $1 + 16 | 0;
    $2 = HEAP32[$1 >> 2];
    HEAP32[$0 + 4 >> 2] = $2;
    HEAP32[$2 >> 2] = $0;
    HEAP32[$1 >> 2] = $3;
    HEAP32[$3 >> 2] = $3;
    $3 = -512;
    break label$1;
   }
   if (($3 | 0) == HEAP32[$3 >> 2]) {
    label$8 : {
     if (!(HEAP8[$1 | 0] & 1)) {
      $5 = HEAP32[$0 >> 2];
      $6 = $5 + 4 | 0;
      break label$8;
     }
     $6 = $0 + 4 | 0;
     $5 = $0;
     $0 = HEAP32[$0 + 4 >> 2];
    }
    $4 = $0;
    HEAP32[$6 >> 2] = $3;
    HEAP32[$1 + 12 >> 2] = $5;
    HEAP32[$4 >> 2] = $3;
    HEAP32[$1 + 16 >> 2] = $4;
   }
   HEAP32[$7 + 12 >> 2] = $2;
   HEAP32[HEAP32[2] >> 2] = $2;
   $3 = 0;
  }
  HEAP8[79440] = $8 & 1;
  return $3;
 }
 
 function finish_wait($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  HEAP32[HEAP32[2] >> 2] = 0;
  $1 = $0 + 12 | 0;
  if (!(HEAP32[$0 + 16 >> 2] == ($1 | 0) ? ($1 | 0) == HEAP32[$0 + 12 >> 2] : 0)) {
   $5 = HEAPU8[79440];
   HEAP8[79440] = 0;
   $2 = $0 + 16 | 0;
   $3 = HEAP32[$2 >> 2];
   $4 = HEAP32[$0 + 12 >> 2];
   HEAP32[$3 >> 2] = $4;
   HEAP32[$4 + 4 >> 2] = $3;
   HEAP32[$0 + 12 >> 2] = $1;
   HEAP32[$2 >> 2] = $1;
   HEAP8[79440] = $5 & 1;
  }
 }
 
 function wake_up_bit($0, $1) {
  var $2 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  HEAP32[$2 + 8 >> 2] = 0;
  HEAP32[$2 >> 2] = $0;
  HEAP32[$2 + 4 >> 2] = $1;
  $0 = (Math_imul($0 << 5 | $1, 1640531527) >>> 24 << 3) + 13568 | 0;
  if (HEAP32[$0 >> 2] != ($0 | 0)) {
   __wake_up($0, 3, $2)
  }
  global$0 = $2 + 16 | 0;
 }
 
 function complete($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $2 = HEAPU8[79444];
  HEAP8[79444] = 0;
  $3 = $0 + 4 | 0;
  $1 = HEAP32[$0 >> 2];
  if (($1 | 0) != -1) {
   HEAP32[$0 >> 2] = $1 + 1
  }
  __wake_up_locked($3);
  HEAP8[79444] = $2 & 1;
 }
 
 function wait_for_completion($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  HEAP8[79444] = 0;
  $1 = HEAP32[$0 >> 2];
  label$1 : {
   if (!$1) {
    HEAP32[$2 + 8 >> 2] = 1;
    $1 = $0 + 8 | 0;
    $3 = HEAP32[$1 >> 2];
    $4 = $2 + 20 | 0;
    HEAP32[$3 >> 2] = $4;
    HEAP32[$1 >> 2] = $4;
    HEAP32[$2 + 24 >> 2] = $3;
    HEAP32[$2 + 16 >> 2] = 77;
    HEAP32[$2 + 12 >> 2] = HEAP32[2];
    HEAP32[$2 + 20 >> 2] = $0 + 4;
    $1 = 2147483647;
    while (1) {
     label$3 : {
      HEAP32[HEAP32[2] >> 2] = 2;
      HEAP8[79444] = 1;
      $1 = schedule_timeout($1);
      HEAP8[79444] = 0;
      if (!$1) {
       break label$3
      }
      if (!HEAP32[$0 >> 2]) {
       continue
      }
     }
     break;
    };
    $1 = HEAP32[$2 + 24 >> 2];
    $3 = HEAP32[$2 + 20 >> 2];
    HEAP32[$1 >> 2] = $3;
    HEAP32[$3 + 4 >> 2] = $1;
    $1 = HEAP32[$0 >> 2];
    if (!$1) {
     break label$1
    }
   }
   if (($1 | 0) == -1) {
    break label$1
   }
   HEAP32[$0 >> 2] = $1 + -1;
  }
  HEAP8[79444] = 1;
  global$0 = $2 + 32 | 0;
 }
 
 function __kmalloc($0, $1) {
  var $2 = 0, $3 = 0;
  $1 = HEAP32[4221] & $1;
  label$1 : {
   label$2 : {
    if ($0 >>> 0 <= 65407) {
     if (!$0) {
      break label$2
     }
     $1 = slob_alloc($0 + 128 | 0, $1, 128);
     if (!$1) {
      break label$1
     }
     HEAP32[$1 >> 2] = $0;
     return $1 + 128 | 0;
    }
    label$4 : {
     label$5 : {
      $0 = $0 + -1 | 0;
      if ($0 >>> 0 < 65536) {
       break label$5
      }
      $0 = $0 & -65536;
      $2 = $0 >>> 0 > 16777215;
      $3 = $2 ? 16 : 8;
      $0 = $2 ? $0 : $0 << 8;
      $2 = $0 >>> 0 > 268435455;
      $3 = $2 ? $3 : $3 + -4 | 0;
      $0 = $2 ? $0 : $0 << 4;
      $2 = $0 >>> 0 > 1073741823;
      $2 = ($2 ? $3 : $3 + -2 | 0) + (($2 ? $0 : $0 << 2) >> 31 ^ -1) | 0;
      if (!$2) {
       break label$5
      }
      $1 = $1 | 16384;
      break label$4;
     }
     $2 = 0;
    }
    $1 = __alloc_pages_nodemask($1, $2);
    if ($1) {
     $0 = HEAP32[$1 + 32 >> 2]
    } else {
     $0 = 0
    }
    return $0;
   }
   return 16;
  }
  return 0;
 }
 
 function slob_alloc($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $8 = global$0 - 16 | 0;
  global$0 = $8;
  label$2 : {
   label$3 : {
    label$4 : {
     label$5 : {
      $4 = $0 >>> 0 >= 256 ? ($0 >>> 0 < 1024 ? 15668 : 15676) : 15660;
      $3 = HEAP32[$4 >> 2];
      if (($4 | 0) != ($3 | 0)) {
       $7 = $0 + 3 >>> 2 | 0;
       while (1) {
        if (HEAPU32[$3 + 20 >> 2] >= $7 >>> 0) {
         $6 = HEAP32[$3 + 4 >> 2];
         $5 = slob_page_alloc($3 + -4 | 0, $0, $2);
         if ($5) {
          break label$5
         }
        }
        $3 = HEAP32[$3 >> 2];
        if (($3 | 0) != ($4 | 0)) {
         continue
        }
        break;
       };
      }
      $5 = 0;
      $3 = __alloc_pages_nodemask($1 & -32769, 0);
      if (!$3) {
       break label$2
      }
      $6 = HEAP32[$3 + 32 >> 2];
      if (!$6) {
       break label$2
      }
      $3 = HEAP32[20646] + Math_imul($6 >>> 16 | 0, 36) | 0;
      $5 = HEAP32[$3 + 4 >> 2];
      $5 = $5 & 1 ? $5 + -1 | 0 : $3;
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] | 512;
      HEAP32[$3 + 16 >> 2] = $6;
      HEAP32[$3 + 24 >> 2] = 16384;
      $5 = $3 + 4 | 0;
      HEAP32[$3 + 4 >> 2] = $5;
      HEAP32[$6 >> 2] = 16384;
      $7 = HEAP32[$4 >> 2];
      HEAP32[$7 + 4 >> 2] = $5;
      HEAP32[$6 + 4 >> 2] = ($6 + 65536 | 0) - ($6 & -65536) >> 2;
      HEAP32[$3 + 4 >> 2] = $7;
      HEAP32[$4 >> 2] = $5;
      HEAP32[$3 + 8 >> 2] = $4;
      $4 = HEAP32[$3 + 4 >> 2];
      $4 = $4 & 1 ? $4 + -1 | 0 : $3;
      HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 8192;
      $5 = slob_page_alloc($3, $0, $2);
      if (!$5) {
       break label$3
      }
      break label$4;
     }
     $3 = HEAP32[$4 + 4 >> 2];
     label$9 : {
      if (($3 | 0) == ($6 | 0)) {
       break label$9
      }
      $7 = HEAP32[$4 >> 2];
      $2 = HEAP32[$6 >> 2];
      if (($7 | 0) == ($2 | 0)) {
       break label$9
      }
      HEAP32[$7 + 4 >> 2] = $3;
      HEAP32[$3 >> 2] = $7;
      $3 = HEAP32[$2 + 4 >> 2];
      HEAP32[$2 + 4 >> 2] = $4;
      HEAP32[$4 >> 2] = $2;
      HEAP32[$3 >> 2] = $4;
      HEAP32[$4 + 4 >> 2] = $3;
     }
    }
    if (!($1 & 32768)) {
     break label$2
    }
    memset($5, 0, $0);
    break label$2;
   }
   HEAP32[$8 + 8 >> 2] = 23369;
   HEAP32[$8 + 4 >> 2] = 330;
   HEAP32[$8 >> 2] = 23351;
   printk(23320, $8);
   abort();
  }
  global$0 = $8 + 16 | 0;
  return $5;
 }
 
 function kfree($0) {
  var $1 = 0, $2 = 0;
  label$1 : {
   if ($0 >>> 0 >= 17) {
    $1 = HEAP32[20646] + Math_imul($0 >>> 16 | 0, 36) | 0;
    $2 = HEAP32[$1 + 4 >> 2];
    if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $1) >> 2] & 512) {
     break label$1
    }
    __free_pages($1, HEAP32[$1 >> 2] & 65536 ? HEAPU8[$1 + 45 | 0] : 0);
   }
   return;
  }
  $0 = $0 + -128 | 0;
  slob_free($0, HEAP32[$0 >> 2] + 128 | 0);
 }
 
 function slob_free($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  label$1 : {
   if ($0 >>> 0 < 17) {
    break label$1
   }
   label$2 : {
    if ($1) {
     $6 = HEAP32[20646];
     $2 = $0 >>> 16 | 0;
     $3 = $6 + Math_imul($2, 36) | 0;
     $5 = HEAP32[$3 + 4 >> 2];
     $11 = HEAP32[($5 & 1 ? $5 + -1 | 0 : $3) >> 2] & 8192;
     $8 = $3 + 4 | 0;
     $9 = $3 + 24 | 0;
     $10 = $1 + 3 | 0;
     $4 = $10 >>> 2 | 0;
     $12 = $4 + HEAP32[$3 + 24 >> 2] | 0;
     if (($12 | 0) == 16384) {
      if ($11) {
       $1 = $6 + Math_imul($2, 36) | 0;
       $4 = $1 + 8 | 0;
       $2 = HEAP32[$4 >> 2];
       HEAP32[$5 + 4 >> 2] = $2;
       HEAP32[$1 + 4 >> 2] = 256;
       HEAP32[$2 >> 2] = $5;
       HEAP32[$4 >> 2] = 512;
       $1 = HEAP32[$8 >> 2];
       $1 = $1 & 1 ? $1 + -1 | 0 : $3;
       HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -8193;
      }
      $1 = HEAP32[$8 >> 2];
      HEAP32[$9 >> 2] = -1;
      $1 = $1 & 1 ? $1 + -1 | 0 : $3;
      HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -513;
      $1 = HEAP32[HEAP32[2] + 804 >> 2];
      if ($1) {
       HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1
      }
      free_pages($0, 0);
      break label$1;
     }
     label$7 : {
      label$8 : {
       label$9 : {
        label$10 : {
         label$11 : {
          label$12 : {
           label$13 : {
            label$14 : {
             label$15 : {
              label$16 : {
               if ($11) {
                HEAP32[$9 >> 2] = $12;
                $2 = ($6 + Math_imul($2, 36) | 0) + 16 | 0;
                $3 = HEAP32[$2 >> 2];
                if ($3 >>> 0 <= $0 >>> 0) {
                 break label$16
                }
                if ((($4 << 2) + $0 | 0) != ($3 | 0)) {
                 break label$9
                }
                $6 = $3 & -65536;
                $1 = HEAP32[$3 >> 2];
                $5 = ($1 | 0) > 1 ? $1 : 1;
                if (($1 | 0) <= -1) {
                 break label$11
                }
                $3 = HEAP32[$3 + 4 >> 2];
                break label$10;
               }
               HEAP32[$9 >> 2] = $4;
               HEAP32[($6 + Math_imul($2, 36) | 0) + 16 >> 2] = $0;
               $5 = ($0 + 65536 & -65536) - ($0 & -65536) >> 2;
               if ($10 >>> 0 < 8) {
                break label$15
               }
               HEAP32[$0 + 4 >> 2] = $5;
               break label$14;
              }
              $6 = $3 & -65536;
              $2 = HEAP32[$3 >> 2];
              if (($2 | 0) <= -1) {
               break label$13
              }
              $1 = HEAP32[$3 + 4 >> 2];
              break label$12;
             }
             $4 = 0 - $5 | 0;
            }
            HEAP32[$0 >> 2] = $4;
            $0 = ($1 | 0) < 256 ? 15660 : ($1 | 0) < 1024 ? 15668 : 15676;
            $4 = HEAP32[$0 >> 2];
            $1 = $6 + Math_imul($2, 36) | 0;
            $2 = $1 + 4 | 0;
            HEAP32[$4 + 4 >> 2] = $2;
            HEAP32[$1 + 4 >> 2] = $4;
            HEAP32[$0 >> 2] = $2;
            HEAP32[$1 + 8 >> 2] = $0;
            $0 = HEAP32[$8 >> 2];
            $0 = $0 & 1 ? $0 + -1 | 0 : $3;
            HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 8192;
            break label$2;
           }
           $1 = 0 - $2 | 0;
          }
          $1 = ($1 << 2) + $6 | 0;
          if ($1 >>> 0 < $0 >>> 0) {
           while (1) {
            $3 = $1;
            $6 = $1 & -65536;
            $5 = $6;
            $2 = HEAP32[$1 >> 2];
            $1 = HEAP32[$1 + 4 >> 2];
            label$20 : {
             if (($2 | 0) > -1) {
              break label$20
             }
             $1 = 0 - $2 | 0;
            }
            $1 = $5 + ($1 << 2) | 0;
            if ($1 >>> 0 < $0 >>> 0) {
             continue
            }
            break;
           }
          }
          if (($2 | 0) > -1) {
           $2 = HEAP32[$3 + 4 >> 2]
          } else {
           $2 = 0 - $2 | 0
          }
          label$24 : {
           label$25 : {
            label$26 : {
             label$27 : {
              if ((($4 << 2) + $0 | 0) != ($1 | 0)) {
               break label$27
              }
              if (!(($2 << 2) + $6 & 65532)) {
               break label$27
              }
              $5 = $1 & -65536;
              $2 = HEAP32[$1 >> 2];
              $4 = (($2 | 0) > 1 ? $2 : 1) + $4 | 0;
              if (($2 | 0) <= -1) {
               break label$26
              }
              $1 = HEAP32[$1 + 4 >> 2];
              break label$25;
             }
             $1 = $1 - ($0 & -65536) >> 2;
             if ($10 >>> 0 >= 8) {
              HEAP32[$0 + 4 >> 2] = $1;
              break label$24;
             }
             $4 = 0 - $1 | 0;
             break label$24;
            }
            $1 = 0 - $2 | 0;
           }
           $1 = (($1 << 2) + $5 | 0) - ($0 & -65536) >> 2;
           if (($4 | 0) >= 2) {
            HEAP32[$0 + 4 >> 2] = $1;
            break label$24;
           }
           $4 = 0 - $1 | 0;
          }
          HEAP32[$0 >> 2] = $4;
          label$30 : {
           label$31 : {
            $1 = HEAP32[$3 >> 2];
            $2 = ($1 | 0) > 1 ? $1 : 1;
            if (($0 | 0) != (($2 << 2) + $3 | 0)) {
             $0 = $0 - $6 >> 2;
             if (($1 | 0) < 2) {
              break label$31
             }
             HEAP32[$3 + 4 >> 2] = $0;
             break label$30;
            }
            $5 = $0 & -65536;
            $1 = $2 + (($4 | 0) > 1 ? $4 : 1) | 0;
            if (($4 | 0) <= -1) {
             break label$8
            }
            $0 = HEAP32[$0 + 4 >> 2];
            break label$7;
           }
           $1 = 0 - $0 | 0;
          }
          HEAP32[$3 >> 2] = $1;
          break label$2;
         }
         $3 = 0 - $1 | 0;
        }
        $4 = $4 + $5 | 0;
        $3 = $6 + ($3 << 2) | 0;
        HEAP32[$2 >> 2] = $3;
       }
       $1 = $3 - ($0 & -65536) >> 2;
       label$33 : {
        if (($4 | 0) >= 2) {
         HEAP32[$0 + 4 >> 2] = $1;
         break label$33;
        }
        $4 = 0 - $1 | 0;
       }
       HEAP32[$0 >> 2] = $4;
       HEAP32[$2 >> 2] = $0;
       break label$2;
      }
      $0 = 0 - $4 | 0;
     }
     $0 = (($0 << 2) + $5 | 0) - $6 >> 2;
     if (($1 | 0) >= 2) {
      HEAP32[$3 + 4 >> 2] = $0;
      HEAP32[$3 >> 2] = $1;
      break label$2;
     }
     HEAP32[$3 >> 2] = 0 - $0;
     break label$2;
    }
    HEAP32[$7 + 8 >> 2] = 23380;
    HEAP32[$7 + 4 >> 2] = 351;
    HEAP32[$7 >> 2] = 23351;
    printk(23320, $7);
    abort();
   }
  }
  global$0 = $7 + 16 | 0;
 }
 
 function kmem_cache_alloc($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $4 = HEAP32[4221] & $1;
  label$1 : {
   label$2 : {
    $1 = HEAP32[$0 + 4 >> 2];
    label$3 : {
     if ($1 >>> 0 <= 65535) {
      $1 = slob_alloc($1, $4, HEAP32[$0 + 8 >> 2]);
      if (!$1) {
       break label$3
      }
      break label$2;
     }
     $1 = $1 + -1 | 0;
     if ($1 >>> 0 >= 65536) {
      $1 = $1 & -65536;
      $2 = $1 >>> 0 > 16777215;
      $3 = $2 ? 16 : 8;
      $1 = $2 ? $1 : $1 << 8;
      $2 = $1 >>> 0 > 268435455;
      $3 = $2 ? $3 : $3 + -4 | 0;
      $1 = $2 ? $1 : $1 << 4;
      $2 = $1 >>> 0 > 1073741823;
      $2 = ($2 ? $3 : $3 + -2 | 0) + (($2 ? $1 : $1 << 2) >> 31 ^ -1) | 0;
     }
     $1 = 0;
     $2 = __alloc_pages_nodemask($4, $2);
     if (!$2) {
      break label$1
     }
     $1 = HEAP32[$2 + 32 >> 2];
     if ($1) {
      break label$2
     }
    }
    return 0;
   }
   $0 = HEAP32[$0 + 32 >> 2];
   if (!$0) {
    break label$1
   }
   FUNCTION_TABLE[$0]($1);
  }
  return $1;
 }
 
 function kmem_cache_free($0, $1) {
  var $2 = 0;
  $2 = HEAP32[$0 + 4 >> 2];
  if (!(HEAPU8[$0 + 14 | 0] & 8)) {
   __kmem_cache_free($1, $2);
   return;
  }
  $0 = $1 + $2 | 0;
  HEAP32[$0 + -4 >> 2] = $2;
  call_rcu($0 + -12 | 0, 78);
 }
 
 function __kmem_cache_free($0, $1) {
  var $2 = 0, $3 = 0;
  if ($1 >>> 0 <= 65535) {
   slob_free($0, $1);
   return;
  }
  $1 = $1 + -1 | 0;
  if ($1 >>> 0 >= 65536) {
   $1 = $1 & -65536;
   $2 = $1 >>> 0 > 16777215;
   $3 = $2 ? 16 : 8;
   $1 = $2 ? $1 : $1 << 8;
   $2 = $1 >>> 0 > 268435455;
   $3 = $2 ? $3 : $3 + -4 | 0;
   $1 = $2 ? $1 : $1 << 4;
   $2 = $1 >>> 0 > 1073741823;
   $2 = ($2 ? $3 : $3 + -2 | 0) + (($2 ? $1 : $1 << 2) >> 31 ^ -1) | 0;
  }
  $1 = HEAP32[HEAP32[2] + 804 >> 2];
  if ($1) {
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + (1 << $2)
  }
  free_pages($0, $2);
 }
 
 function kmem_rcu_free($0) {
  $0 = $0 | 0;
  var $1 = 0;
  $1 = $0;
  $0 = HEAP32[$0 + 8 >> 2];
  __kmem_cache_free($1 + (12 - $0 | 0) | 0, $0);
 }
 
 function slob_page_alloc($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $11 = 0 - $2 | 0;
  $12 = $2 + -1 | 0;
  $8 = $1 + 3 >>> 2 | 0;
  $1 = HEAP32[$0 + 16 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       while (1) {
        if ($2) {
         $5 = $1 + $12 & $11;
         $9 = $5 - $1 >> 2;
        }
        $4 = HEAP32[$1 >> 2];
        $7 = ($4 | 0) > 1 ? $4 : 1;
        if (($7 | 0) < ($8 + $9 | 0)) {
         $6 = $1 & -65536;
         $3 = $6;
         $7 = ($4 | 0) > -1;
         $10 = HEAP32[$1 + 4 >> 2];
         label$9 : {
          if ($7) {
           break label$9
          }
          $10 = 0 - $4 | 0;
         }
         if (!($3 + ($10 << 2) & 65532)) {
          break label$5
         }
         $3 = $1;
         if ($7) {
          $1 = HEAP32[$1 + 4 >> 2]
         } else {
          $1 = 0 - $4 | 0
         }
         $1 = $6 + ($1 << 2) | 0;
         continue;
        }
        break;
       };
       $6 = $1 & -65536;
       if (!$9) {
        break label$4
       }
       if (($4 | 0) <= -1) {
        break label$3
       }
       $3 = HEAP32[$1 + 4 >> 2];
       break label$2;
      }
      return 0;
     }
     $2 = $6;
     $5 = $1;
     break label$1;
    }
    $3 = 0 - $4 | 0;
   }
   $2 = $5 & -65536;
   $3 = (($3 << 2) + $6 | 0) - $2 >> 2;
   $4 = $7 - $9 | 0;
   label$13 : {
    if (($4 | 0) >= 2) {
     HEAP32[$5 + 4 >> 2] = $3;
     break label$13;
    }
    $4 = 0 - $3 | 0;
   }
   HEAP32[$5 >> 2] = $4;
   $3 = $5 - $6 >> 2;
   label$15 : {
    if (($9 | 0) >= 2) {
     HEAP32[$1 + 4 >> 2] = $3;
     break label$15;
    }
    $9 = 0 - $3 | 0;
   }
   HEAP32[$1 >> 2] = $9;
   $4 = HEAP32[$5 >> 2];
   $7 = ($4 | 0) > 1 ? $4 : 1;
   $3 = $1;
  }
  if (($4 | 0) > -1) {
   $1 = HEAP32[$5 + 4 >> 2]
  } else {
   $1 = 0 - $4 | 0
  }
  $1 = ($1 << 2) + $2 | 0;
  label$19 : {
   label$20 : {
    label$21 : {
     label$22 : {
      label$23 : {
       label$24 : {
        label$25 : {
         if (($8 | 0) == ($7 | 0)) {
          if (!$3) {
           break label$25
          }
          $1 = $1 - ($3 & -65536) >> 2;
          $2 = HEAP32[$3 >> 2];
          if (($2 | 0) < 2) {
           break label$23
          }
          HEAP32[$3 + 4 >> 2] = $1;
          HEAP32[$3 >> 2] = $2;
          break label$19;
         }
         if (!$3) {
          break label$24
         }
         $2 = ($8 << 2) + $5 | 0;
         $6 = $2 - ($3 & -65536) >> 2;
         $4 = HEAP32[$3 >> 2];
         if (($4 | 0) < 2) {
          break label$22
         }
         HEAP32[$3 + 4 >> 2] = $6;
         break label$21;
        }
        HEAP32[$0 + 16 >> 2] = $1;
        break label$19;
       }
       $2 = ($8 << 2) + $5 | 0;
       HEAP32[$0 + 16 >> 2] = $2;
       break label$20;
      }
      HEAP32[$3 >> 2] = 0 - $1;
      break label$19;
     }
     $4 = 0 - $6 | 0;
    }
    HEAP32[$3 >> 2] = $4;
   }
   $3 = ($8 << 2) + $5 | 0;
   $2 = $1 - ($2 & -65536) >> 2;
   $1 = $7 - $8 | 0;
   label$27 : {
    if (($1 | 0) >= 2) {
     HEAP32[$3 + 4 >> 2] = $2;
     break label$27;
    }
    $1 = 0 - $2 | 0;
   }
   HEAP32[$3 >> 2] = $1;
  }
  $1 = HEAP32[$0 + 24 >> 2] - $8 | 0;
  HEAP32[$0 + 24 >> 2] = $1;
  if ($1) {
   return $5
  }
  $1 = HEAP32[$0 + 4 >> 2];
  $2 = $0 + 8 | 0;
  $3 = HEAP32[$2 >> 2];
  HEAP32[$1 + 4 >> 2] = $3;
  HEAP32[$0 + 4 >> 2] = 256;
  HEAP32[$3 >> 2] = $1;
  HEAP32[$2 >> 2] = 512;
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = $1 & 1 ? $1 + -1 | 0 : $0;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -8193;
  return $5;
 }
 
 function memblock_find_in_range_node($0, $1, $2, $3, $4, $5) {
  var $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  $8 = $2 >>> 0 > 65536 ? $2 : 65536;
  $2 = $3 ? $3 : HEAP32[3922];
  $3 = $8 >>> 0 > $2 >>> 0 ? $8 : $2;
  label$1 : {
   label$2 : {
    if (!HEAPU8[15684]) {
     break label$2
    }
    if ($3 >>> 0 <= 19448) {
     break label$2
    }
    HEAP32[$6 >> 2] = 0;
    HEAP32[$6 + 4 >> 2] = 0;
    $2 = 0;
    __next_mem_range($6, $4, $5, $6 + 12 | 0, $6 + 8 | 0);
    label$3 : {
     if (HEAP32[$6 >> 2] == -1 & HEAP32[$6 + 4 >> 2] == -1) {
      break label$3
     }
     $9 = $8 >>> 0 > 19448 ? $8 : 19448;
     $11 = $1 + -1 | 0;
     while (1) {
      $2 = HEAP32[$6 + 12 >> 2];
      $2 = $2 >>> 0 > $9 >>> 0 ? $2 : $9;
      $2 = $2 >>> 0 < $3 >>> 0 ? $2 : $3;
      HEAP32[$6 + 12 >> 2] = $2;
      $7 = HEAP32[$6 + 8 >> 2];
      $7 = $7 >>> 0 > $9 >>> 0 ? $7 : $9;
      $7 = $7 >>> 0 < $3 >>> 0 ? $7 : $3;
      HEAP32[$6 + 8 >> 2] = $7;
      $2 = ($2 + -1 | $11) + 1 | 0;
      if ($7 - $2 >>> 0 >= $0 >>> 0 ? $7 >>> 0 > $2 >>> 0 : 0) {
       break label$3
      }
      $2 = 0;
      __next_mem_range($6, $4, $5, $6 + 12 | 0, $6 + 8 | 0);
      if (HEAP32[$6 >> 2] != -1 | HEAP32[$6 + 4 >> 2] != -1) {
       continue
      }
      break;
     };
    }
    if ($2) {
     break label$1
    }
   }
   HEAP32[$6 >> 2] = -1;
   HEAP32[$6 + 4 >> 2] = -1;
   __next_mem_range_rev($6, $4, $5, $6 + 12 | 0, $6 + 8 | 0);
   label$6 : {
    if (HEAP32[$6 >> 2] == -1 & HEAP32[$6 + 4 >> 2] == -1) {
     break label$6
    }
    $2 = 0 - $1 | 0;
    while (1) {
     $1 = HEAP32[$6 + 12 >> 2];
     $1 = $1 >>> 0 > $8 >>> 0 ? $1 : $8;
     $9 = $1 >>> 0 < $3 >>> 0 ? $1 : $3;
     HEAP32[$6 + 12 >> 2] = $9;
     $1 = HEAP32[$6 + 8 >> 2];
     $1 = $1 >>> 0 > $8 >>> 0 ? $1 : $8;
     $1 = $1 >>> 0 < $3 >>> 0 ? $1 : $3;
     HEAP32[$6 + 8 >> 2] = $1;
     if ($1 >>> 0 >= $0 >>> 0) {
      $10 = $2 & $1 - $0;
      if ($10 >>> 0 >= $9 >>> 0) {
       break label$6
      }
     }
     $10 = 0;
     __next_mem_range_rev($6, $4, $5, $6 + 12 | 0, $6 + 8 | 0);
     if (HEAP32[$6 >> 2] != -1 | HEAP32[$6 + 4 >> 2] != -1) {
      continue
     }
     break;
    };
   }
   $2 = $10;
  }
  global$0 = $6 + 16 | 0;
  return $2;
 }
 
 function __next_mem_range($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $5 = HEAP32[$0 + 4 >> 2];
  label$2 : {
   $10 = HEAP32[3923];
   $6 = HEAP32[$0 >> 2];
   if ($10 >>> 0 > $6 >>> 0) {
    $13 = $2 & 4;
    $14 = $2 & 2;
    $2 = $5;
    $15 = HEAP32[3926];
    $16 = (($1 | 0) == 1 ? -1 : $1) + 1 >>> 0 > 1;
    while (1) {
     label$5 : {
      if ($16) {
       break label$5
      }
      $1 = Math_imul($6, 12) + $15 | 0;
      $8 = HEAP32[$1 >> 2];
      if (HEAPU8[$1 + 8 | 0] & 2 ? 0 : $14) {
       break label$5
      }
      if (HEAPU8[$1 + 8 | 0] & 4 ? !$13 : 0) {
       break label$5
      }
      $1 = HEAP32[$1 + 4 >> 2] + $8 | 0;
      $11 = HEAP32[3928];
      $12 = $11 + 1 | 0;
      if ($2 >>> 0 >= $12 >>> 0) {
       break label$5
      }
      $5 = (HEAP32[3931] + Math_imul($2, 12) | 0) + -12 | 0;
      while (1) {
       $7 = $2 >>> 0 < $11 >>> 0 ? HEAP32[$5 + 12 >> 2] : -1;
       $9 = $2 ? HEAP32[$5 + 4 >> 2] + HEAP32[$5 >> 2] | 0 : 0;
       if ($9 >>> 0 >= $1 >>> 0) {
        break label$5
       }
       if ($8 >>> 0 < $7 >>> 0) {
        break label$2
       }
       $5 = $5 + 12 | 0;
       $2 = $2 + 1 | 0;
       if ($2 >>> 0 < $12 >>> 0) {
        continue
       }
       break;
      };
     }
     $6 = $6 + 1 | 0;
     if ($6 >>> 0 < $10 >>> 0) {
      continue
     }
     break;
    };
   }
   HEAP32[$0 >> 2] = -1;
   HEAP32[$0 + 4 >> 2] = -1;
   return;
  }
  if ($3) {
   HEAP32[$3 >> 2] = $8 >>> 0 > $9 >>> 0 ? $8 : $9
  }
  if ($4) {
   HEAP32[$4 >> 2] = $1 >>> 0 < $7 >>> 0 ? $1 : $7
  }
  HEAP32[$0 >> 2] = $6 + ($1 >>> 0 <= $7 >>> 0);
  HEAP32[$0 + 4 >> 2] = ($1 >>> 0 > $7 >>> 0) + $2;
 }
 
 function __next_mem_range_rev($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0;
  $6 = HEAP32[$0 >> 2];
  $5 = HEAP32[$0 + 4 >> 2];
  label$1 : {
   if (!(($6 | 0) == -1 & ($5 | 0) == -1)) {
    break label$1
   }
   $6 = HEAP32[3923] + -1 | 0;
   $5 = HEAP32[3928];
   if (15712) {
    break label$1
   }
   $5 = 0;
  }
  label$5 : {
   if (($6 | 0) >= 0) {
    $10 = $2 & 4;
    $11 = $2 & 2;
    $12 = HEAP32[3926];
    $13 = (($1 | 0) == 1 ? -1 : $1) + 1 >>> 0 > 1;
    while (1) {
     $1 = $6;
     label$8 : {
      if ($13) {
       break label$8
      }
      $2 = Math_imul($1, 12) + $12 | 0;
      $7 = HEAP32[$2 >> 2];
      if (HEAPU8[$2 + 8 | 0] & 2 ? 0 : $11) {
       break label$8
      }
      if (HEAPU8[$2 + 8 | 0] & 4 ? !$10 : 0) {
       break label$8
      }
      $9 = HEAP32[$2 + 4 >> 2] + $7 | 0;
      if (($5 | 0) < 0) {
       break label$8
      }
      $2 = (HEAP32[3931] + Math_imul($5, 12) | 0) + -12 | 0;
      $14 = HEAP32[3928];
      $6 = $5;
      while (1) {
       $5 = $6 ? HEAP32[$2 + 4 >> 2] + HEAP32[$2 >> 2] | 0 : 0;
       $8 = $6 >>> 0 < $14 >>> 0 ? HEAP32[$2 + 12 >> 2] : -1;
       if ($8 >>> 0 > $7 >>> 0) {
        if ($9 >>> 0 > $5 >>> 0) {
         break label$5
        }
        $2 = $2 + -12 | 0;
        $8 = ($6 | 0) > 0;
        $5 = $6 + -1 | 0;
        $6 = $5;
        if ($8) {
         continue
        }
        break label$8;
       }
       break;
      };
      $5 = $6;
     }
     $6 = $1 + -1 | 0;
     if (($1 | 0) > 0) {
      continue
     }
     break;
    };
   }
   HEAP32[$0 >> 2] = -1;
   HEAP32[$0 + 4 >> 2] = -1;
   return;
  }
  if ($3) {
   HEAP32[$3 >> 2] = $7 >>> 0 > $5 >>> 0 ? $7 : $5
  }
  if ($4) {
   HEAP32[$4 >> 2] = $9 >>> 0 < $8 >>> 0 ? $9 : $8
  }
  HEAP32[$0 >> 2] = $1 - ($7 >>> 0 >= $5 >>> 0);
  HEAP32[$0 + 4 >> 2] = $6 - ($7 >>> 0 < $5 >>> 0);
 }
 
 function memblock_add_range($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $7 = global$0 - 32 | 0;
  global$0 = $7;
  label$1 : {
   $4 = $1 ^ -1;
   $11 = $4 >>> 0 > $2 >>> 0 ? $2 : $4;
   if (!$11) {
    break label$1
   }
   label$2 : {
    label$3 : {
     label$4 : {
      $8 = HEAP32[$0 + 12 >> 2];
      if (HEAP32[$8 + 4 >> 2]) {
       $10 = $1 + $11 | 0;
       $12 = $0 + 4 | 0;
       $13 = $0 + 8 | 0;
       $6 = HEAP32[$0 >> 2];
       if ($6) {
        break label$3
       }
       break label$4;
      }
      HEAP32[$8 + 8 >> 2] = 0;
      HEAP32[$8 >> 2] = $1;
      HEAP32[$0 + 8 >> 2] = $11;
      HEAP32[$8 + 4 >> 2] = $11;
      break label$1;
     }
     $3 = 0;
     break label$2;
    }
    $3 = 1;
   }
   while (1) {
    label$7 : {
     label$8 : {
      label$9 : {
       label$10 : {
        label$11 : {
         if ($3) {
          $3 = 0;
          $5 = $6;
          $4 = $1;
          $9 = 0;
          $2 = $8;
          while (1) {
           $14 = HEAP32[$2 >> 2];
           if ($14 >>> 0 < $10 >>> 0) {
            $15 = $14 + HEAP32[$2 + 4 >> 2] | 0;
            if ($15 >>> 0 > $4 >>> 0) {
             label$17 : {
              if ($14 >>> 0 <= $4 >>> 0) {
               break label$17
              }
              $9 = $9 + 1 | 0;
              if (!$16) {
               break label$17
              }
              if ($5 >>> 0 >= HEAPU32[$12 >> 2]) {
               break label$9
              }
              $2 = Math_imul($3, 12) + $8 | 0;
              memmove($2 + 12 | 0, $2, Math_imul($5 - $3 | 0, 12));
              HEAP32[$2 + 8 >> 2] = 0;
              $5 = $14 - $4 | 0;
              HEAP32[$2 + 4 >> 2] = $5;
              HEAP32[$2 >> 2] = $4;
              $6 = HEAP32[$0 >> 2] + 1 | 0;
              HEAP32[$0 >> 2] = $6;
              HEAP32[$13 >> 2] = $5 + HEAP32[$13 >> 2];
              $3 = $3 + 1 | 0;
              $8 = HEAP32[$0 + 12 >> 2];
              $5 = $6;
             }
             $4 = $15 >>> 0 < $10 >>> 0 ? $15 : $10;
            }
            $3 = $3 + 1 | 0;
            $2 = Math_imul($3, 12) + $8 | 0;
            if ($5 >>> 0 > $3 >>> 0) {
             continue
            }
           }
           break;
          };
          if ($10 >>> 0 > $4 >>> 0) {
           break label$11
          }
          break label$10;
         }
         $5 = 0;
         $9 = 0;
         $3 = 0;
         $4 = $1;
         if ($10 >>> 0 <= $4 >>> 0) {
          break label$10
         }
        }
        $9 = $9 + 1 | 0;
        if (!$16) {
         break label$10
        }
        if ($5 >>> 0 >= HEAPU32[$12 >> 2]) {
         break label$8
        }
        $2 = Math_imul($3, 12) + $8 | 0;
        memmove($2 + 12 | 0, $2, Math_imul($5 - $3 | 0, 12));
        HEAP32[$2 + 8 >> 2] = 0;
        $5 = $10 - $4 | 0;
        HEAP32[$2 + 4 >> 2] = $5;
        HEAP32[$2 >> 2] = $4;
        $6 = HEAP32[$0 >> 2] + 1 | 0;
        HEAP32[$0 >> 2] = $6;
        HEAP32[$13 >> 2] = $5 + HEAP32[$13 >> 2];
       }
       $3 = 0;
       if (!$9) {
        break label$1
       }
       if (!$16) {
        if ($9 + $6 >>> 0 <= HEAPU32[$12 >> 2]) {
         break label$7
        }
        while (1) {
         if ((memblock_double_array($0, $1, $11) | 0) >= 0) {
          $6 = HEAP32[$0 >> 2];
          if ($9 + $6 >>> 0 > HEAPU32[$12 >> 2]) {
           continue
          }
          break label$7;
         }
         break;
        };
        $3 = -12;
        break label$1;
       }
       memblock_merge_regions($0);
       break label$1;
      }
      HEAP32[$7 + 24 >> 2] = 23834;
      HEAP32[$7 + 20 >> 2] = 551;
      HEAP32[$7 + 16 >> 2] = 23823;
      printk(23792, $7 + 16 | 0);
      abort();
     }
     HEAP32[$7 + 8 >> 2] = 23834;
     HEAP32[$7 + 4 >> 2] = 551;
     HEAP32[$7 >> 2] = 23823;
     printk(23792, $7);
     abort();
    }
    $8 = HEAP32[$0 + 12 >> 2];
    $16 = 1;
    if (!$6) {
     continue
    }
    $3 = 1;
    continue;
   };
  }
  global$0 = $7 + 32 | 0;
  return $3;
 }
 
 function memblock_double_array($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $3 = global$0 - 128 | 0;
  global$0 = $3;
  $10 = HEAPU32[20650] > 2;
  $5 = -1;
  label$1 : {
   label$2 : {
    if (!HEAPU8[82536]) {
     break label$2
    }
    $11 = HEAP32[$0 + 4 >> 2];
    $4 = Math_imul($11, 24);
    $7 = $4 + 65535 & -65536;
    label$3 : {
     label$4 : {
      if ($10) {
       $1 = __kmalloc($4, 6291648);
       break label$4;
      }
      $6 = HEAP32[3922];
      $8 = HEAPU8[82528];
      HEAP32[$3 + 124 >> 2] = $7;
      $9 = $2;
      $2 = ($0 | 0) == 15712;
      $9 = $2 ? $9 : 0;
      $2 = $2 ? $1 : 0;
      $12 = $9 + $2 | 0;
      $1 = memblock_find_in_range_node($7, 65536, $12, $6, -1, $8 ? 2 : 0);
      label$6 : {
       if (!($8 ? !$1 : 0)) {
        HEAP32[$3 + 108 >> 2] = $1;
        break label$6;
       }
       HEAP32[$3 + 96 >> 2] = $3 + 124;
       printk(23406, $3 + 96 | 0);
       $1 = memblock_find_in_range_node(HEAP32[$3 + 124 >> 2], 65536, $12, $6, -1, 0);
       HEAP32[$3 + 108 >> 2] = $1;
      }
      if (!$9) {
       break label$3
      }
      if ($1) {
       break label$3
      }
      $1 = HEAP32[3922];
      $6 = HEAPU8[82528];
      HEAP32[$3 + 124 >> 2] = $7;
      $2 = $2 >>> 0 < $1 >>> 0 ? $2 : $1;
      $1 = memblock_find_in_range_node($7, 65536, 0, $2, -1, $6 ? 2 : 0);
      if ($1) {
       break label$4
      }
      if (!$6) {
       break label$4
      }
      HEAP32[$3 + 80 >> 2] = $3 + 124;
      printk(23406, $3 + 80 | 0);
      $1 = memblock_find_in_range_node(HEAP32[$3 + 124 >> 2], 65536, 0, $2, -1, 0);
     }
     HEAP32[$3 + 108 >> 2] = $1;
    }
    label$9 : {
     label$10 : {
      if ($1) {
       $5 = Math_imul($11, 12);
       $6 = ($0 | 0) == 15692 ? 82552 : 82556;
       HEAP32[$3 + 104 >> 2] = ($1 + $4 | 0) + -1;
       if (HEAP32[20633]) {
        break label$10
       }
       break label$9;
      }
      $1 = HEAP32[$0 + 16 >> 2];
      $0 = HEAP32[$0 + 4 >> 2];
      HEAP32[$3 + 4 >> 2] = $0;
      HEAP32[$3 >> 2] = $1;
      HEAP32[$3 + 8 >> 2] = $0 << 1;
      printk(23857, $3);
      break label$2;
     }
     $2 = HEAP32[$0 + 4 >> 2];
     HEAP32[$3 + 64 >> 2] = HEAP32[$0 + 16 >> 2];
     HEAP32[$3 + 68 >> 2] = $2 << 1;
     HEAP32[$3 + 76 >> 2] = $3 + 104;
     HEAP32[$3 + 72 >> 2] = $3 + 108;
     printk(23922, $3 - -64 | 0);
    }
    memcpy($1, HEAP32[$0 + 12 >> 2], $5);
    $4 = $0 + 4 | 0;
    memset(Math_imul(HEAP32[$4 >> 2], 12) + $1 | 0, 0, $5);
    $2 = HEAP32[$0 + 12 >> 2];
    HEAP32[$0 + 12 >> 2] = $1;
    HEAP32[$4 >> 2] = HEAP32[$4 >> 2] << 1;
    label$12 : {
     if (HEAP32[$6 >> 2]) {
      kfree($2);
      break label$12;
     }
     if (($2 | 0) == 79456) {
      break label$12
     }
     if (($2 | 0) == 80992) {
      break label$12
     }
     HEAP32[$3 + 116 >> 2] = $2;
     $0 = $5 + 65535 & -65536;
     HEAP32[$3 + 112 >> 2] = ($0 + $2 | 0) + -1;
     if (HEAP32[20633]) {
      HEAP32[$3 + 56 >> 2] = 0;
      HEAP32[$3 + 52 >> 2] = $3 + 112;
      HEAP32[$3 + 48 >> 2] = $3 + 116;
      printk(23458, $3 + 48 | 0);
      $2 = HEAP32[$3 + 116 >> 2];
     }
     if (memblock_isolate_range(15712, $2, $0, $3 + 124 | 0, $3 + 120 | 0)) {
      break label$12
     }
     $1 = HEAP32[$3 + 120 >> 2];
     $8 = HEAP32[$3 + 124 >> 2];
     if (($1 | 0) <= ($8 | 0)) {
      break label$12
     }
     $5 = Math_imul($1, 12);
     $2 = 0 - $1 | 0;
     $0 = HEAP32[3928];
     while (1) {
      $4 = HEAP32[3931] + $5 | 0;
      HEAP32[3930] = HEAP32[3930] - HEAP32[$4 + -8 >> 2];
      memmove($4 + -12 | 0, $4, Math_imul($0 + $2 | 0, 12));
      $0 = HEAP32[3928] + -1 | 0;
      HEAP32[3928] = $0;
      $1 = $1 + -1 | 0;
      if (!$0) {
       HEAP32[3928] = 1;
       $0 = HEAP32[3931];
       HEAP32[$0 + 8 >> 2] = 0;
       HEAP32[$0 >> 2] = 0;
       HEAP32[$0 + 4 >> 2] = 0;
       $0 = 1;
      }
      $5 = $5 + -12 | 0;
      $2 = $2 + 1 | 0;
      if (($1 | 0) > ($8 | 0)) {
       continue
      }
      break;
     };
    }
    if (!$10) {
     $0 = HEAP32[$3 + 108 >> 2];
     HEAP32[$3 + 124 >> 2] = $0;
     HEAP32[$3 + 120 >> 2] = ($0 + $7 | 0) + -1;
     if (HEAP32[20633]) {
      HEAP32[$3 + 40 >> 2] = 0;
      HEAP32[$3 + 36 >> 2] = $3 + 120;
      HEAP32[$3 + 32 >> 2] = $3 + 124;
      printk(23493, $3 + 32 | 0);
      $0 = HEAP32[$3 + 124 >> 2];
     }
     if (memblock_add_range(15712, $0, $7)) {
      break label$1
     }
    }
    HEAP32[$6 >> 2] = $10;
    $5 = 0;
   }
   global$0 = $3 + 128 | 0;
   return $5;
  }
  HEAP32[$3 + 24 >> 2] = 23968;
  HEAP32[$3 + 20 >> 2] = 492;
  HEAP32[$3 + 16 >> 2] = 23823;
  printk(23792, $3 + 16 | 0);
  abort();
 }
 
 function memblock_merge_regions($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  label$1 : {
   $2 = HEAP32[$0 >> 2];
   if (($2 | 0) != 1) {
    while (1) {
     $6 = $3 + 1 | 0;
     label$4 : {
      label$5 : {
       $4 = HEAP32[$0 + 12 >> 2];
       $5 = $4 + Math_imul($3, 12) | 0;
       $8 = HEAP32[$5 + 4 >> 2];
       $9 = $8 + HEAP32[$5 >> 2] | 0;
       $7 = $5 + 12 | 0;
       $10 = HEAP32[$7 >> 2];
       if (($9 | 0) != ($10 | 0)) {
        break label$5
       }
       $4 = $4 + Math_imul($6, 12) | 0;
       if (HEAP32[$5 + 8 >> 2] != HEAP32[$4 + 8 >> 2]) {
        break label$5
       }
       HEAP32[$5 + 4 >> 2] = HEAP32[$4 + 4 >> 2] + $8;
       memmove($7, $7 + 12 | 0, Math_imul((-2 - $3 | 0) + $2 | 0, 12));
       $2 = HEAP32[$0 >> 2] + -1 | 0;
       HEAP32[$0 >> 2] = $2;
       $6 = $3;
       break label$4;
      }
      if ($9 >>> 0 > $10 >>> 0) {
       break label$1
      }
     }
     $3 = $6;
     if ($3 >>> 0 < $2 + -1 >>> 0) {
      continue
     }
     break;
    }
   }
   global$0 = $1 + 16 | 0;
   return;
  }
  HEAP32[$1 + 8 >> 2] = 23990;
  HEAP32[$1 + 4 >> 2] = 519;
  HEAP32[$1 >> 2] = 23823;
  printk(23792, $1);
  abort();
 }
 
 function memblock_isolate_range($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $9 = global$0 - 32 | 0;
  global$0 = $9;
  HEAP32[$4 >> 2] = 0;
  HEAP32[$3 >> 2] = 0;
  label$1 : {
   label$2 : {
    $7 = $1 ^ -1;
    $7 = $7 >>> 0 > $2 >>> 0 ? $2 : $7;
    label$3 : {
     if (!$7) {
      break label$3
     }
     $13 = $1 + $7 | 0;
     $14 = $0 + 4 | 0;
     label$4 : {
      while (1) {
       $2 = HEAP32[$0 >> 2];
       if ($2 + 2 >>> 0 <= HEAPU32[$14 >> 2]) {
        break label$4
       }
       if ((memblock_double_array($0, $1, $7) | 0) >= 0) {
        continue
       }
       break;
      };
      $16 = -12;
      break label$3;
     }
     if ($2) {
      $14 = $0 + 8 | 0;
      $15 = $0 + 12 | 0;
      $12 = HEAP32[$15 >> 2];
      $5 = $12;
      while (1) {
       $7 = HEAP32[$5 >> 2];
       if ($13 >>> 0 <= $7 >>> 0) {
        break label$3
       }
       $6 = HEAP32[$5 + 4 >> 2];
       $10 = $6 + $7 | 0;
       if ($10 >>> 0 > $1 >>> 0) {
        label$9 : {
         if ($7 >>> 0 < $1 >>> 0) {
          HEAP32[$5 >> 2] = $1;
          $11 = $6;
          $6 = $1 - $7 | 0;
          HEAP32[$5 + 4 >> 2] = $11 - $6;
          $10 = $0 + 8 | 0;
          HEAP32[$10 >> 2] = HEAP32[$10 >> 2] - $6;
          if ($2 >>> 0 >= HEAPU32[$0 + 4 >> 2]) {
           break label$2
          }
          $11 = HEAP32[$5 + 8 >> 2];
          $5 = Math_imul($8, 12) + $12 | 0;
          memmove($5 + 12 | 0, $5, Math_imul($2 - $8 | 0, 12));
          HEAP32[$5 + 8 >> 2] = $11;
          HEAP32[$5 + 4 >> 2] = $6;
          HEAP32[$5 >> 2] = $7;
          $2 = HEAP32[$0 >> 2] + 1 | 0;
          HEAP32[$0 >> 2] = $2;
          $5 = $6 + HEAP32[$10 >> 2] | 0;
          $6 = $14;
          break label$9;
         }
         if ($10 >>> 0 > $13 >>> 0) {
          HEAP32[$5 >> 2] = $13;
          $11 = $6;
          $6 = $13 - $7 | 0;
          HEAP32[$5 + 4 >> 2] = $11 - $6;
          $10 = $0 + 8 | 0;
          HEAP32[$10 >> 2] = HEAP32[$10 >> 2] - $6;
          if ($2 >>> 0 >= HEAPU32[$0 + 4 >> 2]) {
           break label$1
          }
          $11 = HEAP32[$5 + 8 >> 2];
          $5 = Math_imul($8, 12) + $12 | 0;
          memmove($5 + 12 | 0, $5, Math_imul($2 - $8 | 0, 12));
          HEAP32[$5 + 8 >> 2] = $11;
          HEAP32[$5 + 4 >> 2] = $6;
          HEAP32[$5 >> 2] = $7;
          $2 = HEAP32[$0 >> 2] + 1 | 0;
          HEAP32[$0 >> 2] = $2;
          $8 = $8 + -1 | 0;
          $5 = $6 + HEAP32[$10 >> 2] | 0;
          $6 = $14;
          break label$9;
         }
         if (!HEAP32[$4 >> 2]) {
          HEAP32[$3 >> 2] = $8
         }
         $5 = $8 + 1 | 0;
         $6 = $4;
        }
        HEAP32[$6 >> 2] = $5;
       }
       $12 = HEAP32[$15 >> 2];
       $8 = $8 + 1 | 0;
       $5 = $12 + Math_imul($8, 12) | 0;
       if ($2 >>> 0 > $8 >>> 0) {
        continue
       }
       break;
      };
      break label$3;
     }
    }
    global$0 = $9 + 32 | 0;
    return $16;
   }
   HEAP32[$9 + 8 >> 2] = 23834;
   HEAP32[$9 + 4 >> 2] = 551;
   HEAP32[$9 >> 2] = 23823;
   printk(23792, $9);
   abort();
  }
  HEAP32[$9 + 24 >> 2] = 23834;
  HEAP32[$9 + 20 >> 2] = 551;
  HEAP32[$9 + 16 >> 2] = 23823;
  printk(23792, $9 + 16 | 0);
  abort();
 }
 
 function memblock_reserve($0, $1) {
  var $2 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  HEAP32[$2 + 28 >> 2] = $0;
  HEAP32[$2 + 24 >> 2] = ($0 + $1 | 0) + -1;
  if (HEAP32[20633]) {
   HEAP32[$2 + 8 >> 2] = 0;
   HEAP32[$2 + 4 >> 2] = $2 + 24;
   HEAP32[$2 >> 2] = $2 + 28;
   printk(23493, $2);
   $0 = HEAP32[$2 + 28 >> 2];
  }
  $0 = memblock_add_range(15712, $0, $1);
  global$0 = $2 + 32 | 0;
  return $0;
 }
 
 function memblock_clear_hotplug() {
  var $0 = 0, $1 = 0, $2 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  if (!memblock_isolate_range(15692, 0, -1, $1 + 12 | 0, $1 + 8 | 0)) {
   $0 = HEAP32[$1 + 12 >> 2];
   $2 = HEAP32[$1 + 8 >> 2];
   if (($0 | 0) < ($2 | 0)) {
    $2 = $2 - $0 | 0;
    $0 = (HEAP32[3926] + Math_imul($0, 12) | 0) + 8 | 0;
    while (1) {
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -2;
     $0 = $0 + 12 | 0;
     $2 = $2 + -1 | 0;
     if ($2) {
      continue
     }
     break;
    };
   }
   memblock_merge_regions(15692);
  }
  global$0 = $1 + 16 | 0;
 }
 
 function memblock_alloc_internal($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  HEAP32[$5 + 28 >> 2] = $0;
  HEAP32[$5 + 16 >> 2] = 24031;
  $6 = HEAPU8[82528];
  printk(24013, $5 + 16 | 0);
  label$1 : {
   label$2 : {
    if (HEAPU32[20650] <= 2) {
     $7 = ($4 | 0) == 1 ? -1 : $4;
     if ($1) {
      break label$2
     }
     dump_stack();
     $1 = 128;
     break label$2;
    }
    $2 = __kmalloc($0, 4227072);
    break label$1;
   }
   $4 = $6 ? 2 : 0;
   $0 = HEAP32[3922];
   $3 = $0 >>> 0 < $3 >>> 0 ? $0 : $3;
   $6 = ($7 | 0) == -1;
   while (1) {
    $0 = $2;
    $2 = memblock_find_in_range_node(HEAP32[$5 + 28 >> 2], $1, $0, $3, $7, $4);
    if ($2) {
     if (!memblock_reserve($2, HEAP32[$5 + 28 >> 2])) {
      break label$1
     }
    }
    label$6 : {
     if ($6) {
      break label$6
     }
     $2 = memblock_find_in_range_node(HEAP32[$5 + 28 >> 2], $1, $0, $3, -1, $4);
     if (!$2) {
      break label$6
     }
     if (!memblock_reserve($2, HEAP32[$5 + 28 >> 2])) {
      break label$1
     }
    }
    $2 = 0;
    if ($0) {
     continue
    }
    if (!($4 & 2)) {
     break label$1
    }
    HEAP32[$5 >> 2] = $5 + 28;
    printk(23406, $5);
    $4 = $4 & -3;
    continue;
   };
  }
  global$0 = $5 + 32 | 0;
  return $2;
 }
 
 function memblock_alloc_try_nid_nopanic($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $3 = global$0 - 48 | 0;
  global$0 = $3;
  HEAP32[$3 + 40 >> 2] = 0;
  HEAP32[$3 + 44 >> 2] = 0;
  $5 = $0;
  $6 = $1;
  if (HEAP32[20633]) {
   HEAP32[$3 + 36 >> 2] = 0;
   HEAP32[$3 + 24 >> 2] = $2;
   $4 = $3 + 16 | 0;
   HEAP32[$4 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = 0;
   HEAP32[$3 >> 2] = 23592;
   HEAP32[$3 + 32 >> 2] = $3 + 40;
   HEAP32[$3 + 28 >> 2] = $3 + 44;
   HEAP32[$3 + 8 >> 2] = $0;
   HEAP32[$3 + 12 >> 2] = 0;
   printk(23528, $3);
   $4 = HEAP32[$3 + 40 >> 2];
   $1 = HEAP32[$3 + 44 >> 2];
  } else {
   $1 = 0
  }
  $1 = memblock_alloc_internal($5, $6, $1, $4, $2);
  if ($1) {
   memset($1, 0, $0)
  }
  global$0 = $3 + 48 | 0;
  return $1;
 }
 
 function __memblock_dump_all() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $0 = global$0 - 160 | 0;
  global$0 = $0;
  printk(23723, 0);
  HEAP32[$0 + 100 >> 2] = 15720;
  HEAP32[$0 + 96 >> 2] = 15700;
  printk(23750, $0 + 96 | 0);
  HEAP32[$0 + 84 >> 2] = HEAP32[3923];
  HEAP32[$0 + 80 >> 2] = HEAP32[3927];
  printk(24055, $0 + 80 | 0);
  if (HEAP32[3923]) {
   $3 = HEAP32[3926];
   $5 = $0 + 68 | 0;
   while (1) {
    $1 = $0 + 136 | 0;
    HEAP32[$1 >> 2] = 0;
    HEAP32[$1 + 4 >> 2] = 0;
    $1 = $0 + 128 | 0;
    HEAP32[$1 >> 2] = 0;
    HEAP32[$1 + 4 >> 2] = 0;
    HEAP32[$0 + 120 >> 2] = 0;
    HEAP32[$0 + 124 >> 2] = 0;
    HEAP32[$0 + 112 >> 2] = 0;
    HEAP32[$0 + 116 >> 2] = 0;
    $1 = HEAP32[$3 + 4 >> 2];
    $4 = HEAP32[$3 >> 2];
    HEAP32[$0 + 72 >> 2] = HEAP32[$3 + 8 >> 2];
    HEAP32[$0 + 156 >> 2] = $4;
    HEAP32[$0 + 148 >> 2] = $1;
    HEAP32[$5 >> 2] = $0 + 112;
    HEAP32[$0 - -64 >> 2] = $0 + 148;
    HEAP32[$0 + 48 >> 2] = HEAP32[3927];
    HEAP32[$0 + 152 >> 2] = ($1 + $4 | 0) + -1;
    HEAP32[$0 + 60 >> 2] = $0 + 152;
    HEAP32[$0 + 56 >> 2] = $0 + 156;
    HEAP32[$0 + 52 >> 2] = $2;
    printk(24075, $0 + 48 | 0);
    $2 = $2 + 1 | 0;
    $3 = HEAP32[3926] + Math_imul($2, 12) | 0;
    if ($2 >>> 0 < HEAPU32[3923]) {
     continue
    }
    break;
   };
  }
  HEAP32[$0 + 36 >> 2] = HEAP32[3928];
  HEAP32[$0 + 32 >> 2] = HEAP32[3932];
  printk(24055, $0 + 32 | 0);
  if (HEAP32[3928]) {
   $3 = HEAP32[3931];
   $5 = $0 + 20 | 0;
   $2 = 0;
   while (1) {
    $1 = $0 + 136 | 0;
    HEAP32[$1 >> 2] = 0;
    HEAP32[$1 + 4 >> 2] = 0;
    $1 = $0 + 128 | 0;
    HEAP32[$1 >> 2] = 0;
    HEAP32[$1 + 4 >> 2] = 0;
    HEAP32[$0 + 120 >> 2] = 0;
    HEAP32[$0 + 124 >> 2] = 0;
    HEAP32[$0 + 112 >> 2] = 0;
    HEAP32[$0 + 116 >> 2] = 0;
    $1 = HEAP32[$3 + 4 >> 2];
    $4 = HEAP32[$3 >> 2];
    HEAP32[$0 + 24 >> 2] = HEAP32[$3 + 8 >> 2];
    HEAP32[$0 + 156 >> 2] = $4;
    HEAP32[$0 + 148 >> 2] = $1;
    HEAP32[$5 >> 2] = $0 + 112;
    HEAP32[$0 + 16 >> 2] = $0 + 148;
    HEAP32[$0 >> 2] = HEAP32[3932];
    HEAP32[$0 + 152 >> 2] = ($1 + $4 | 0) + -1;
    HEAP32[$0 + 12 >> 2] = $0 + 152;
    HEAP32[$0 + 8 >> 2] = $0 + 156;
    HEAP32[$0 + 4 >> 2] = $2;
    printk(24075, $0);
    $2 = $2 + 1 | 0;
    $3 = HEAP32[3931] + Math_imul($2, 12) | 0;
    if ($2 >>> 0 < HEAPU32[3928]) {
     continue
    }
    break;
   };
  }
  global$0 = $0 + 160 | 0;
 }
 
 function reset_all_zones_managed_pages() {
  var $0 = 0;
  if (!HEAPU8[141164]) {
   $0 = 141272;
   if ($0) {
    while (1) {
     HEAP32[$0 + 40 >> 2] = 0;
     HEAP32[$0 + 556 >> 2] = 0;
     $0 = 0;
     if ($0) {
      continue
     }
     break;
    }
   }
   HEAP8[141164] = 1;
  }
 }
 
 function free_low_memory_core_early() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  memblock_clear_hotplug();
  HEAP32[$0 >> 2] = 0;
  HEAP32[$0 + 4 >> 2] = 0;
  label$1 : {
   if (!HEAP32[3928]) {
    break label$1
   }
   HEAP32[$0 >> 2] = 1;
   HEAP32[$0 + 4 >> 2] = 0;
   $1 = HEAP32[3931];
   $2 = HEAP32[$1 >> 2];
   HEAP32[$0 + 12 >> 2] = $2;
   $3 = (HEAP32[$1 + 4 >> 2] + $2 | 0) + -1 | 0;
   HEAP32[$0 + 8 >> 2] = $3;
   $5 = 16;
   $4 = 1;
   $1 = 0;
   while (1) {
    reserve_bootmem_region($2, $3);
    if (!$1 & $4 >>> 0 >= HEAPU32[3928] | $1 >>> 0 > 0) {
     break label$1
    }
    $2 = HEAP32[3931] + $5 | 0;
    $3 = HEAP32[$2 >> 2];
    $2 = HEAP32[$2 + -4 >> 2];
    HEAP32[$0 + 12 >> 2] = $2;
    $3 = ($2 + $3 | 0) + -1 | 0;
    HEAP32[$0 + 8 >> 2] = $3;
    $5 = $5 + 12 | 0;
    $4 = $4 + 1 | 0;
    if ($4 >>> 0 < 1) {
     $1 = $1 + 1 | 0
    }
    if (($4 | 0) != -1 | ($1 | 0) != -1) {
     continue
    }
    break;
   };
  }
  HEAP32[$0 >> 2] = 0;
  HEAP32[$0 + 4 >> 2] = 0;
  __next_mem_range($0, -1, 0, $0 + 12 | 0, $0 + 8 | 0);
  if (!(HEAP32[$0 >> 2] == -1 & HEAP32[$0 + 4 >> 2] == -1)) {
   while (1) {
    $1 = __free_memory_core(HEAP32[$0 + 12 >> 2], HEAP32[$0 + 8 >> 2]);
    __next_mem_range($0, -1, 0, $0 + 12 | 0, $0 + 8 | 0);
    $6 = $1 + $6 | 0;
    if (HEAP32[$0 >> 2] != -1 | HEAP32[$0 + 4 >> 2] != -1) {
     continue
    }
    break;
   }
  }
  global$0 = $0 + 16 | 0;
  return $6;
 }
 
 function __free_memory_core($0, $1) {
  var $2 = 0;
  $1 = $1 >>> 16 | 0;
  $2 = HEAP32[20635];
  $1 = $1 >>> 0 < $2 >>> 0 ? $1 : $2;
  $0 = $0 + 65535 >>> 16 | 0;
  if ($1 >>> 0 <= $0 >>> 0) {
   return 0
  }
  __free_pages_memory($0, $1);
  return $1 - $0 | 0;
 }
 
 function __free_pages_memory($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  if ($0 >>> 0 < $1 >>> 0) {
   while (1) {
    $4 = $0 & 65535;
    $2 = $4 ? $0 : $0 >>> 16 | 0;
    $3 = $2 & 255;
    $2 = $3 ? $2 : $2 >>> 8 | 0;
    $5 = $2 & 15;
    $2 = $5 ? $2 : $2 >>> 4 | 0;
    $6 = $2 & 3;
    $7 = (($6 ? $2 : $2 >>> 2 | 0) ^ -1) & 1;
    $2 = !$4 << 4;
    $2 = $3 ? $2 : $2 | 8;
    $2 = $5 ? $2 : $2 | 4;
    $2 = $7 + ($6 ? $2 : $2 | 2) | 0;
    $3 = ($2 >>> 0 < 10 ? $2 : 10) + 1 | 0;
    while (1) {
     $3 = $3 + -1 | 0;
     $2 = (1 << $3) + $0 | 0;
     if ($2 >>> 0 > $1 >>> 0) {
      continue
     }
     break;
    };
    __free_pages_boot_core(HEAP32[20646] + Math_imul($0, 36) | 0, $3);
    $0 = $2;
    if ($2 >>> 0 < $1 >>> 0) {
     continue
    }
    break;
   }
  }
 }
 
 function __insert_vmap_area($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  $2 = 82568;
  label$1 : {
   $1 = HEAP32[20642];
   label$2 : {
    if (!$1) {
     break label$2
    }
    $5 = HEAP32[$0 >> 2];
    $6 = $0 + 4 | 0;
    while (1) {
     $3 = $1;
     if ($5 >>> 0 < HEAPU32[$1 + -8 >> 2]) {
      $2 = $3 + 8 | 0;
      $1 = HEAP32[$2 >> 2];
      if ($1) {
       continue
      }
      break label$2;
     }
     if (HEAPU32[$6 >> 2] <= HEAPU32[$3 + -12 >> 2]) {
      break label$1
     }
     $2 = $3 + 4 | 0;
     $1 = HEAP32[$2 >> 2];
     if ($1) {
      continue
     }
     break;
    };
   }
   $1 = $0 + 16 | 0;
   HEAP32[$1 >> 2] = 0;
   HEAP32[$1 + 4 >> 2] = 0;
   HEAP32[$0 + 12 >> 2] = $3;
   $1 = $0 + 12 | 0;
   HEAP32[$2 >> 2] = $1;
   rb_insert_color($1, 82568);
   $3 = $0 + 24 | 0;
   $1 = rb_prev($1);
   label$5 : {
    if ($1) {
     HEAP32[$0 + 28 >> 2] = $1 + 12;
     $2 = HEAP32[$1 + 12 >> 2];
     HEAP32[$3 >> 2] = $2;
     HEAP32[$1 + 12 >> 2] = $3;
     break label$5;
    }
    HEAP32[$0 + 28 >> 2] = 16144;
    $2 = HEAP32[4036];
    HEAP32[$3 >> 2] = $2;
    HEAP32[4036] = $3;
   }
   HEAP32[$2 + 4 >> 2] = $3;
   global$0 = $4 + 16 | 0;
   return;
  }
  HEAP32[$4 + 8 >> 2] = 24168;
  HEAP32[$4 + 4 >> 2] = 378;
  HEAP32[$4 >> 2] = 24152;
  printk(24121, $4);
  abort();
 }
 
 function vmalloc_init() {
  var $0 = 0, $1 = 0, $2 = 0;
  HEAP32[4038] = 16152;
  HEAP32[4039] = 16152;
  HEAP32[4040] = 0;
  HEAP32[4041] = -32;
  HEAP32[4044] = 79;
  HEAP32[4043] = 16168;
  HEAP32[4042] = 16168;
  $0 = HEAP32[35292];
  if ($0) {
   while (1) {
    $1 = __kmalloc(48, 4227072);
    HEAP32[$1 + 8 >> 2] = 4;
    HEAP32[$1 + 36 >> 2] = $0;
    $2 = HEAP32[$0 + 4 >> 2];
    HEAP32[$1 >> 2] = $2;
    HEAP32[$1 + 4 >> 2] = HEAP32[$0 + 8 >> 2] + $2;
    __insert_vmap_area($1);
    $0 = HEAP32[$0 >> 2];
    if ($0) {
     continue
    }
    break;
   }
  }
  HEAP8[82560] = 1;
  HEAP32[20641] = 2139095040;
 }
 
 function free_work($0) {
  $0 = $0 | 0;
  var $1 = 0;
  $1 = $0 + -4 | 0;
  $0 = HEAP32[$1 >> 2];
  HEAP32[$1 >> 2] = 0;
  if ($0) {
   while (1) {
    $1 = HEAP32[$0 >> 2];
    __vunmap($0);
    $0 = $1;
    if ($0) {
     continue
    }
    break;
   }
  }
 }
 
 function __vunmap($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  label$1 : {
   label$2 : {
    label$3 : {
     if (!$0) {
      break label$3
     }
     if ($0 & 65535) {
      break label$3
     }
     $1 = HEAP32[20642];
     label$4 : {
      label$5 : {
       if (!$1) {
        break label$5
       }
       while (1) {
        $3 = $1 + -12 | 0;
        if (HEAPU32[$3 >> 2] > $0 >>> 0) {
         $1 = HEAP32[$1 + 8 >> 2];
         if ($1) {
          continue
         }
         break label$5;
        }
        if (HEAPU32[$1 + -8 >> 2] > $0 >>> 0) {
         break label$4
        }
        $1 = HEAP32[$1 + 4 >> 2];
        if ($1) {
         continue
        }
        break;
       };
      }
      $3 = 0;
     }
     $3 = HEAP32[$3 + 36 >> 2];
     if (!$3) {
      break label$3
     }
     $1 = HEAP32[20642];
     label$9 : {
      if (!$1) {
       break label$9
      }
      label$10 : {
       while (1) {
        if (HEAPU32[$1 + -12 >> 2] > $0 >>> 0) {
         $1 = HEAP32[$1 + 8 >> 2];
         if ($1) {
          continue
         }
         break label$9;
        }
        if (HEAPU32[$1 + -8 >> 2] > $0 >>> 0) {
         break label$10
        }
        $1 = HEAP32[$1 + 4 >> 2];
        if ($1) {
         continue
        }
        break;
       };
       break label$9;
      }
      $0 = $1 + -4 | 0;
      if (HEAPU8[$0 | 0] & 4) {
       break label$1
      }
     }
     if (HEAP32[$3 + 20 >> 2]) {
      $5 = $3 + 16 | 0;
      $6 = $3 + 20 | 0;
      $1 = 0;
      $0 = 0;
      while (1) {
       $4 = HEAP32[HEAP32[$5 >> 2] + $1 >> 2];
       if (!$4) {
        break label$2
       }
       __free_pages($4, 0);
       $1 = $1 + 4 | 0;
       $0 = $0 + 1 | 0;
       if ($0 >>> 0 < HEAPU32[$6 >> 2]) {
        continue
       }
       break;
      };
     }
     __inlined_func$kvfree : {
      label$11 : {
       $0 = HEAP32[$3 + 16 >> 2];
       if ($0 >>> 0 < 2136997888) {
        break label$11
       }
       if ($0 >>> 0 > 2139095039) {
        break label$11
       }
       vfree($0);
       break __inlined_func$kvfree;
      }
      kfree($0);
     }
     kfree($3);
    }
    global$0 = $2 + 32 | 0;
    return;
   }
   HEAP32[$2 + 8 >> 2] = 24205;
   HEAP32[$2 + 4 >> 2] = 1525;
   HEAP32[$2 >> 2] = 24152;
   printk(24121, $2);
   abort();
  }
  HEAP32[$1 + 24 >> 2] = 0;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -7 | 2;
  HEAP32[$2 + 24 >> 2] = 24187;
  HEAP32[$2 + 20 >> 2] = 124;
  HEAP32[$2 + 16 >> 2] = 24152;
  printk(24121, $2 + 16 | 0);
  abort();
 }
 
 function vfree($0) {
  var $1 = 0, $2 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  label$1 : {
   if (!(HEAP32[1] & 1048576)) {
    if (!$0) {
     break label$1
    }
    if (!(HEAP32[1] & 2096896)) {
     __vunmap($0);
     break label$1;
    }
    $2 = HEAP32[4040];
    HEAP32[$0 >> 2] = $2;
    HEAP32[4040] = $0;
    if ($2) {
     break label$1
    }
    queue_work_on(1, HEAP32[16393], 16164);
    break label$1;
   }
   HEAP32[$1 + 8 >> 2] = 24162;
   HEAP32[$1 + 4 >> 2] = 1586;
   HEAP32[$1 >> 2] = 24152;
   printk(24121, $1);
   abort();
  }
  global$0 = $1 + 16 | 0;
 }
 
 function __put_anon_vma($0) {
  var $1 = 0;
  $1 = HEAP32[$0 >> 2];
  if (HEAP32[$1 + 4 >> 2]) {
   __down_write(HEAP32[$0 >> 2] + 4 | 0);
   __up_write(HEAP32[$0 >> 2] + 4 | 0);
  }
  kmem_cache_free(HEAP32[20643], $0);
  label$2 : {
   if (($0 | 0) != ($1 | 0)) {
    $0 = HEAP32[$1 + 16 >> 2] + -1 | 0;
    HEAP32[$1 + 16 >> 2] = $0;
    if (!$0) {
     break label$2
    }
   }
   return;
  }
  if (HEAP32[HEAP32[$1 >> 2] + 4 >> 2]) {
   __down_write(HEAP32[$1 >> 2] + 4 | 0);
   __up_write(HEAP32[$1 >> 2] + 4 | 0);
  }
  kmem_cache_free(HEAP32[20643], $1);
 }
 
 function page_lock_anon_vma_read($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = HEAP32[$0 + 12 >> 2];
  label$1 : {
   label$2 : {
    if (($1 & 3) != 1) {
     break label$2
    }
    if (!page_mapped($0)) {
     break label$2
    }
    $1 = $1 + -1 | 0;
    $2 = HEAP32[$1 >> 2] + 4 | 0;
    if (__down_read_trylock($2)) {
     if (page_mapped($0)) {
      break label$1
     }
     __up_read($2);
     break label$2;
    }
    $3 = HEAP32[$1 + 16 >> 2];
    if (!$3) {
     break label$2
    }
    $2 = $1 + 16 | 0;
    HEAP32[$2 >> 2] = $3 + 1;
    label$4 : {
     if (page_mapped($0)) {
      __down_read(HEAP32[$1 >> 2] + 4 | 0);
      $0 = HEAP32[$2 >> 2] + -1 | 0;
      HEAP32[$2 >> 2] = $0;
      if ($0) {
       break label$1
      }
      __up_read(HEAP32[$1 >> 2] + 4 | 0);
      break label$4;
     }
     $0 = HEAP32[$2 >> 2] + -1 | 0;
     HEAP32[$2 >> 2] = $0;
     if ($0) {
      break label$2
     }
    }
    __put_anon_vma($1);
   }
   $1 = 0;
  }
  return $1 | 0;
 }
 
 function page_referenced($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $4 = global$0 - 48 | 0;
  global$0 = $4;
  label$1 : {
   label$2 : {
    if (HEAP32[$0 >> 2] & 65536) {
     break label$2
    }
    if (HEAP32[$0 + 4 >> 2] & 1) {
     break label$2
    }
    $5 = HEAP32[$0 + 24 >> 2] + 1 | 0;
    break label$1;
   }
   $5 = __page_mapcount($0);
  }
  HEAP32[$4 + 36 >> 2] = $2;
  HEAP32[$4 + 28 >> 2] = 0;
  HEAP32[$4 + 32 >> 2] = 0;
  HEAP32[$4 + 24 >> 2] = $5;
  $5 = 0;
  HEAP32[$3 >> 2] = 0;
  HEAP32[$4 + 16 >> 2] = 0;
  HEAP32[$4 + 12 >> 2] = 80;
  HEAP32[$4 + 8 >> 2] = 0;
  HEAP32[$4 + 4 >> 2] = 81;
  HEAP32[$4 >> 2] = $4 + 24;
  label$3 : {
   if (!page_mapped($0)) {
    break label$3
   }
   $6 = HEAP32[$0 + 4 >> 2];
   if (!(HEAP32[($6 & 1 ? $6 + -1 | 0 : $0) + 12 >> 2] & -4)) {
    break label$3
   }
   label$4 : {
    if ($1) {
     break label$4
    }
    $5 = 1;
    $1 = HEAP32[$0 + 4 >> 2];
    if (HEAP8[($1 & 1 ? $1 + -1 | 0 : $0) + 12 | 0] & 1) {
     break label$4
    }
    $1 = HEAP32[$0 + 4 >> 2];
    $6 = $1 & 1 ? $1 + -1 | 0 : $0;
    $1 = HEAP32[$6 >> 2];
    HEAP32[$4 + 44 >> 2] = $1;
    if (HEAP32[$4 + 44 >> 2] & 1) {
     break label$3
    }
    HEAP32[$6 >> 2] = $1 | 1;
    $1 = $1 & 1;
    if ($1) {
     break label$3
    }
    $7 = $1 ^ 1;
   }
   if ($2) {
    HEAP32[$4 + 16 >> 2] = 82
   }
   $1 = HEAP32[$0 + 4 >> 2];
   label$6 : {
    if (!(HEAP8[($1 & 1 ? $1 + -1 | 0 : $0) + 12 | 0] & 1)) {
     rmap_walk_file($0, $4);
     break label$6;
    }
    rmap_walk_anon($0, $4);
   }
   HEAP32[$3 >> 2] = HEAP32[$4 + 32 >> 2];
   if ($7) {
    unlock_page($0)
   }
   $5 = HEAP32[$4 + 28 >> 2];
  }
  global$0 = $4 + 48 | 0;
  return $5;
 }
 
 function page_referenced_one($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $4 = global$0 - 32 | 0;
  global$0 = $4;
  HEAP32[$4 + 20 >> 2] = 0;
  HEAP32[$4 + 24 >> 2] = 0;
  HEAP32[$4 + 12 >> 2] = 0;
  HEAP32[$4 + 16 >> 2] = 0;
  HEAP32[$4 + 8 >> 2] = $2;
  HEAP32[$4 + 4 >> 2] = $1;
  HEAP32[$4 >> 2] = $0;
  label$1 : {
   label$2 : {
    label$3 : {
     if (!page_vma_mapped_walk($4)) {
      break label$3
     }
     $0 = 0;
     $2 = $1 + 41 | 0;
     $6 = $4 + 16 | 0;
     $7 = $1 + 40 | 0;
     while (1) {
      if (HEAPU8[$2 | 0] & 32) {
       break label$2
      }
      $5 = HEAP32[$6 >> 2];
      label$5 : {
       if (!$5) {
        break label$5
       }
       if (!ptep_clear_flush_young($1, $5)) {
        break label$5
       }
       $0 = ((HEAP32[$7 >> 2] >>> 15 ^ -1) & 1) + $0 | 0;
      }
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + -1;
      if (page_vma_mapped_walk($4)) {
       continue
      }
      break;
     };
     if (!$0) {
      break label$3
     }
     HEAP32[$3 + 4 >> 2] = HEAP32[$3 + 4 >> 2] + 1;
     HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] | HEAP32[$1 + 40 >> 2];
    }
    $3 = HEAP32[$3 >> 2] != 0;
    break label$1;
   }
   label$6 : {
    if (!HEAP32[$4 + 20 >> 2]) {
     break label$6
    }
   }
   HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] | 8192;
   $3 = 0;
  }
  global$0 = $4 + 32 | 0;
  return $3 | 0;
 }
 
 function rmap_walk_file($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $4 = page_mapping($0);
  if ($4) {
   $5 = HEAP32[$0 + 16 >> 2];
   __down_read($4 + 28 | 0);
   $2 = vma_interval_tree_iter_first($4 + 20 | 0, $5, $5);
   label$3 : {
    if (!$2) {
     break label$3
    }
    $8 = $1 + 4 | 0;
    $9 = $1 + 8 | 0;
    while (1) {
     $6 = HEAP32[$0 + 16 >> 2];
     $10 = HEAP32[$2 + 76 >> 2];
     $3 = HEAP32[$2 >> 2];
     _cond_resched();
     $7 = HEAP32[$1 + 16 >> 2];
     label$5 : {
      if ($7) {
       if (FUNCTION_TABLE[$7]($2, HEAP32[$1 >> 2])) {
        break label$5
       }
      }
      $6 = $3 + ($6 - $10 << 16) | 0;
      if (!FUNCTION_TABLE[HEAP32[$8 >> 2]]($0, $2, $6 >>> 0 > $3 >>> 0 ? $6 : $3, HEAP32[$1 >> 2])) {
       break label$3
      }
      $3 = HEAP32[$9 >> 2];
      if (!$3) {
       break label$5
      }
      if (FUNCTION_TABLE[$3]($0)) {
       break label$3
      }
     }
     $2 = vma_interval_tree_iter_next($2, $5, $5);
     if ($2) {
      continue
     }
     break;
    };
   }
   __up_read($4 + 28 | 0);
  }
 }
 
 function rmap_walk_anon($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  label$1 : {
   $3 = HEAP32[$1 + 12 >> 2];
   label$2 : {
    if ($3) {
     $3 = FUNCTION_TABLE[$3]($0) | 0;
     if ($3) {
      break label$2
     }
     break label$1;
    }
    $3 = page_anon_vma($0);
    if (!$3) {
     break label$1
    }
    __down_read(HEAP32[$3 >> 2] + 4 | 0);
   }
   $8 = $0 + 16 | 0;
   $5 = HEAP32[$8 >> 2];
   $4 = anon_vma_interval_tree_iter_first($3 + 28 | 0, $5, $5);
   label$5 : {
    if (!$4) {
     break label$5
    }
    $9 = $1 + 4 | 0;
    $10 = $1 + 8 | 0;
    while (1) {
     $11 = HEAP32[$8 >> 2];
     $2 = HEAP32[$4 >> 2];
     $12 = HEAP32[$2 + 76 >> 2];
     $7 = HEAP32[$2 >> 2];
     _cond_resched();
     $6 = HEAP32[$1 + 16 >> 2];
     label$7 : {
      if ($6) {
       if (FUNCTION_TABLE[$6]($2, HEAP32[$1 >> 2])) {
        break label$7
       }
      }
      $6 = $2;
      $2 = ($11 - $12 << 16) + $7 | 0;
      if (!FUNCTION_TABLE[HEAP32[$9 >> 2]]($0, $6, $2 >>> 0 > $7 >>> 0 ? $2 : $7, HEAP32[$1 >> 2])) {
       break label$5
      }
      $2 = HEAP32[$10 >> 2];
      if (!$2) {
       break label$7
      }
      if (FUNCTION_TABLE[$2]($0)) {
       break label$5
      }
     }
     $4 = anon_vma_interval_tree_iter_next($4, $5, $5);
     if ($4) {
      continue
     }
     break;
    };
   }
   __up_read(HEAP32[$3 >> 2] + 4 | 0);
  }
 }
 
 function page_mkclean($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 48 | 0;
  global$0 = $1;
  HEAP32[$1 + 44 >> 2] = 0;
  HEAP32[$1 + 40 >> 2] = 83;
  HEAP32[$1 + 32 >> 2] = 0;
  HEAP32[$1 + 36 >> 2] = 0;
  HEAP32[$1 + 28 >> 2] = 84;
  $2 = HEAP32[$0 + 4 >> 2];
  $2 = HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2];
  HEAP32[$1 + 24 >> 2] = $1 + 44;
  if ($2 & 1) {
   label$2 : {
    if (!page_mapped($0)) {
     break label$2
    }
    if (!page_mapping($0)) {
     break label$2
    }
    $3 = HEAP32[$0 + 4 >> 2];
    label$3 : {
     if (!(HEAP8[($3 & 1 ? $3 + -1 | 0 : $0) + 12 | 0] & 1)) {
      rmap_walk_file($0, $1 + 24 | 0);
      break label$3;
     }
     rmap_walk_anon($0, $1 + 24 | 0);
    }
    $3 = HEAP32[$1 + 44 >> 2];
   }
   global$0 = $1 + 48 | 0;
   return $3;
  }
  HEAP32[$1 + 8 >> 2] = 24252;
  HEAP32[$1 + 4 >> 2] = 975;
  HEAP32[$1 >> 2] = 24245;
  printk(24214, $1);
  abort();
 }
 
 function invalid_mkclean_vma($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  return !(HEAPU8[$0 + 40 | 0] & 8) | 0;
 }
 
 function page_mkclean_one($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $4 = global$0 - 32 | 0;
  global$0 = $4;
  HEAP32[$4 + 20 >> 2] = 0;
  HEAP32[$4 + 24 >> 2] = 1;
  HEAP32[$4 + 12 >> 2] = 0;
  HEAP32[$4 + 16 >> 2] = 0;
  HEAP32[$4 + 8 >> 2] = $2;
  HEAP32[$4 + 4 >> 2] = $1;
  HEAP32[$4 >> 2] = $0;
  if (page_vma_mapped_walk($4)) {
   $2 = $4 + 16 | 0;
   while (1) {
    $0 = HEAP32[$2 >> 2];
    label$3 : {
     if (!$0) {
      break label$3
     }
     if (!(HEAPU8[$0 | 0] & 36)) {
      break label$3
     }
     (wasm2js_i32$0 = $0, wasm2js_i32$1 = ptep_clear_flush($1, $0) & -37), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
     HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + 1;
    }
    if (page_vma_mapped_walk($4)) {
     continue
    }
    break;
   };
  }
  global$0 = $4 + 32 | 0;
  return 1;
 }
 
 function page_remove_rmap($0) {
  var $1 = 0;
  label$1 : {
   $1 = HEAP32[$0 + 4 >> 2];
   label$2 : {
    if (!(HEAP8[($1 & 1 ? $1 + -1 | 0 : $0) + 12 | 0] & 1)) {
     $1 = HEAP32[$0 + 24 >> 2];
     HEAP32[$0 + 24 >> 2] = $1 + -1;
     if (($1 | 0) > 0) {
      break label$2
     }
     HEAP32[35629] = HEAP32[35629] + -1;
     HEAP32[20683] = HEAP32[20683] + -1;
     $1 = HEAP32[$0 + 4 >> 2];
     if (!(HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 2097152)) {
      break label$2
     }
     break label$1;
    }
    $1 = HEAP32[$0 + 24 >> 2];
    HEAP32[$0 + 24 >> 2] = $1 + -1;
    if (($1 | 0) > 0) {
     break label$2
    }
    HEAP32[35628] = HEAP32[35628] + -1;
    HEAP32[20682] = HEAP32[20682] + -1;
    $1 = HEAP32[$0 + 4 >> 2];
    if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 2097152) {
     break label$1
    }
   }
   return;
  }
  clear_page_mlock($0);
 }
 
 function try_to_unmap($0) {
  var $1 = 0, $2 = 0;
  $1 = global$0 - 32 | 0;
  global$0 = $1;
  HEAP32[$1 + 24 >> 2] = 0;
  HEAP32[$1 + 20 >> 2] = 80;
  HEAP32[$1 + 16 >> 2] = 85;
  HEAP32[$1 + 12 >> 2] = 86;
  HEAP32[$1 + 8 >> 2] = 64;
  $2 = HEAP32[$0 + 4 >> 2];
  label$2 : {
   if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) + 12 >> 2] & 1)) {
    rmap_walk_file($0, $1 + 8 | 0);
    break label$2;
   }
   rmap_walk_anon($0, $1 + 8 | 0);
  }
  label$6 : {
   label$7 : {
    if (HEAP32[$0 >> 2] & 65536) {
     break label$7
    }
    if (HEAP32[$0 + 4 >> 2] & 1) {
     break label$7
    }
    $0 = HEAP32[$0 + 24 >> 2] + 1 | 0;
    break label$6;
   }
   $0 = __page_mapcount($0);
  }
  global$0 = $1 + 32 | 0;
  return !$0;
 }
 
 function page_mapcount_is_zero($0) {
  $0 = $0 | 0;
  label$1 : {
   if (HEAP32[$0 >> 2] & 65536) {
    break label$1
   }
   if (HEAP32[$0 + 4 >> 2] & 1) {
    break label$1
   }
   return !(HEAP32[$0 + 24 >> 2] + 1) | 0;
  }
  return !__page_mapcount($0) | 0;
 }
 
 function try_to_unmap_one($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  $8 = HEAP32[$1 + 32 >> 2];
  HEAP32[$5 + 20 >> 2] = 0;
  HEAP32[$5 + 24 >> 2] = 0;
  HEAP32[$5 + 12 >> 2] = 0;
  HEAP32[$5 + 16 >> 2] = 0;
  HEAP32[$5 + 8 >> 2] = $2;
  HEAP32[$5 + 4 >> 2] = $1;
  HEAP32[$5 >> 2] = $0;
  $12 = $3 & 2;
  label$1 : {
   if ($12) {
    $2 = 1;
    if (!(HEAPU8[$1 + 41 | 0] & 32)) {
     break label$1
    }
   }
   $2 = 1;
   if (!page_vma_mapped_walk($5)) {
    break label$1
   }
   $9 = $8 + 68 | 0;
   $16 = $3 & 16;
   $17 = $3 & 8;
   $18 = $1 + 41 | 0;
   $19 = $8 + 340 | 0;
   $10 = $8 + 344 | 0;
   $20 = $8 + 352 | 0;
   $13 = $8 + 76 | 0;
   $3 = $0 + 4 | 0;
   $21 = $8 + 72 | 0;
   $14 = $8 + 348 | 0;
   label$3 : {
    label$4 : {
     label$5 : {
      label$6 : {
       while (1) {
        label$7 : {
         label$9 : {
          label$10 : {
           if (!$17) {
            if (HEAPU8[$18 | 0] & 32) {
             break label$6
            }
            if ($12) {
             break label$10
            }
           }
           $7 = $5 + 16 | 0;
           $2 = HEAP32[$7 >> 2];
           $6 = HEAPU16[$2 + 2 >> 1];
           $4 = ($0 - HEAP32[20646] | 0) / -36 | 0;
           $5 + 8 | 0;
           if (!$16) {
            if (ptep_clear_flush_young($1, $2)) {
             break label$5
            }
            $2 = HEAP32[$7 >> 2];
           }
           $4 = Math_imul($4, 36);
           $15 = ptep_clear_flush($1, $2);
           if ($15 & 32) {
            set_page_dirty($0)
           }
           $11 = Math_imul($6, 36);
           $6 = $0 + $4 | 0;
           $2 = HEAP32[$10 >> 2];
           $4 = HEAP32[$19 >> 2];
           $22 = (($2 | 0) > 0 ? $2 : 0) + (($4 | 0) > 0 ? $4 : 0) | 0;
           $4 = HEAP32[$20 >> 2];
           $4 = $22 + (($4 | 0) > 0 ? $4 : 0) | 0;
           if (HEAPU32[$13 >> 2] < $4 >>> 0) {
            HEAP32[$13 >> 2] = $4
           }
           $11 = $6 + $11 | 0;
           $6 = HEAP32[$3 >> 2];
           label$15 : {
            if (!(HEAP8[($6 & 1 ? $6 + -1 | 0 : $0) + 12 | 0] & 1)) {
             $2 = HEAP32[$3 >> 2];
             $2 = ((HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] << 12 >> 31 & 12) + $8 | 0) + 340 | 0;
             HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + -1;
             break label$15;
            }
            $6 = HEAP32[$3 >> 2];
            if (HEAP32[($6 & 1 ? $6 + -1 | 0 : $0) >> 2] & 524288) {
             break label$3
            }
            $6 = HEAP32[$11 + 20 >> 2];
            $4 = HEAP32[$3 >> 2];
            if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $0) >> 2] & 524288)) {
             $7 = HEAP32[$3 >> 2];
             if (HEAP32[($7 & 1 ? $7 + -1 | 0 : $0) >> 2] & 8) {
              break label$4
             }
             HEAP32[$10 >> 2] = $2 + -1;
             break label$15;
            }
            $4 = $10;
            if (($9 | 0) == HEAP32[$9 >> 2]) {
             if (($9 | 0) == HEAP32[$9 >> 2]) {
              $2 = HEAP32[3951];
              HEAP32[$2 + 4 >> 2] = $9;
              HEAP32[$9 >> 2] = $2;
              HEAP32[3951] = $9;
              HEAP32[$21 >> 2] = 15804;
             }
             $2 = HEAP32[$10 >> 2];
            }
            HEAP32[$4 >> 2] = $2 + -1;
            HEAP32[$14 >> 2] = HEAP32[$14 >> 2] + 1;
            HEAP32[HEAP32[$7 >> 2] >> 2] = $6 >>> 26 & 31 | $6 << 13;
           }
           page_remove_rmap($11);
           $2 = HEAP32[$3 >> 2];
           $2 = $2 & 1 ? $2 + -1 | 0 : $0;
           $7 = HEAP32[$2 + 28 >> 2] + -1 | 0;
           HEAP32[$2 + 28 >> 2] = $7;
           if (!$7) {
            break label$9
           }
          }
          if (page_vma_mapped_walk($5)) {
           continue
          }
          break label$7;
         }
         __put_page($2);
         if (page_vma_mapped_walk($5)) {
          continue
         }
        }
        break;
       };
       $2 = 1;
       break label$1;
      }
      mlock_vma_page($0);
      $2 = 0;
      break label$1;
     }
     $2 = 0;
     break label$1;
    }
    HEAP32[HEAP32[$5 + 16 >> 2] >> 2] = $15;
    $1 = HEAP32[$0 + 4 >> 2];
    $0 = $1 & 1 ? $1 + -1 | 0 : $0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 524288;
    $2 = 0;
    break label$1;
   }
   $2 = 0;
   if (!HEAP32[$5 + 20 >> 2]) {
    break label$1
   }
  }
  global$0 = $5 + 32 | 0;
  return $2 | 0;
 }
 
 function invalid_migration_vma($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  return (HEAP32[$0 + 40 >> 2] & 98560) == 98560 | 0;
 }
 
 function ptep_clear_flush_young($0, $1) {
  var $2 = 0;
  $2 = HEAP32[$1 >> 2];
  if ($2 & 16) {
   HEAP32[$1 >> 2] = $2 & -17;
   local_flush_tlb_page($0);
   $0 = 1;
  } else {
   $0 = 0
  }
  return $0;
 }
 
 function ptep_clear_flush($0, $1) {
  var $2 = 0;
  $2 = HEAP32[$1 >> 2];
  HEAP32[$1 >> 2] = 0;
  local_flush_tlb_page($0);
  return $2;
 }
 
 function page_vma_mapped_walk($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $1 = HEAP32[$0 + 16 >> 2];
  label$1 : {
   label$2 : {
    if (HEAP32[$0 + 12 >> 2]) {
     if ($1) {
      break label$2
     }
     $1 = 0;
     if (!HEAP32[$0 + 20 >> 2]) {
      break label$1
     }
     return 0;
    }
    if ($1) {
     break label$2
    }
    $1 = 0;
    HEAP32[$0 + 12 >> 2] = 0;
    $2 = HEAP32[0];
    if (!$2) {
     break label$1
    }
    $3 = $0 + 16 | 0;
    $1 = $2 & -65536 | HEAP32[$0 + 8 >> 2] >>> 14 & 124;
    HEAP32[$3 >> 2] = $1;
    $4 = HEAP32[$0 + 24 >> 2];
    label$4 : {
     if ($4 & 1) {
      break label$4
     }
     $5 = HEAP32[$1 >> 2];
     $1 = $5 & 512;
     if (!($4 & 2)) {
      if (!$1) {
       break label$2
      }
      break label$4;
     }
     if (!$5) {
      break label$2
     }
     if ($1) {
      break label$2
     }
    }
    HEAP32[$0 + 20 >> 2] = (HEAP32[20646] + Math_imul($2 >>> 16 | 0, 36) | 0) + 20;
    if (HEAPU8[$0 + 24 | 0] & 2) {
     break label$2
    }
    $2 = HEAP32[HEAP32[$3 >> 2] >> 2];
    if (!($2 & 512)) {
     break label$2
    }
    $1 = 1;
    if (((HEAP32[$0 >> 2] - HEAP32[20646] | 0) / 36 | 0) == ($2 >>> 16 | 0)) {
     break label$1
    }
   }
   $1 = 0;
   if (!HEAP32[$0 + 20 >> 2]) {
    break label$1
   }
  }
  return $1;
 }
 
 function clear_page_mlock($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  $1 = HEAP32[$0 + 4 >> 2];
  $3 = $1 & 1 ? $1 + -1 | 0 : $0;
  $1 = HEAP32[$3 >> 2];
  HEAP32[$2 + 12 >> 2] = $1;
  label$1 : {
   if (!(HEAP32[$2 + 12 >> 2] & 2097152)) {
    break label$1
   }
   HEAP32[$3 >> 2] = $1 & -2097153;
   if (!($1 & 2097152)) {
    break label$1
   }
   HEAP32[20663] = HEAP32[20663] + -1;
   $1 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141768 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
   if (isolate_lru_page($0)) {
    $1 = HEAP32[$0 + 4 >> 2];
    $1 & 1 ? $1 + -1 | 0 : $0;
    break label$1;
   }
   putback_lru_page($0);
  }
  global$0 = $2 + 16 | 0;
 }
 
 function mlock_vma_page($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  $1 = HEAP32[$0 + 4 >> 2];
  if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 1) {
   $1 = HEAP32[$0 + 4 >> 2];
   $3 = $1 & 1 ? $1 + -1 | 0 : $0;
   $1 = HEAP32[$3 >> 2];
   HEAP32[$2 + 12 >> 2] = $1;
   label$2 : {
    if (HEAP32[$2 + 12 >> 2] & 2097152) {
     break label$2
    }
    HEAP32[$3 >> 2] = $1 | 2097152;
    if ($1 & 2097152) {
     break label$2
    }
    HEAP32[20663] = HEAP32[20663] + 1;
    $1 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141768 | 0;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
    if (isolate_lru_page($0)) {
     break label$2
    }
    putback_lru_page($0);
   }
   global$0 = $2 + 16 | 0;
   return;
  }
  HEAP32[$2 + 8 >> 2] = 24304;
  HEAP32[$2 + 4 >> 2] = 91;
  HEAP32[$2 >> 2] = 24296;
  printk(24265, $2);
  abort();
 }
 
 function __dump_page($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $2 = global$0 - 96 | 0;
  global$0 = $2;
  label$1 : {
   if (HEAP32[$0 >> 2] == -1) {
    HEAP32[$2 + 16 >> 2] = $0;
    printk(25976, $2 + 16 | 0);
    break label$1;
   }
   $4 = HEAP32[$0 + 4 >> 2];
   $3 = 0;
   label$3 : {
    if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $0) >> 2] & 512) {
     break label$3
    }
    label$4 : {
     if (HEAP32[$0 >> 2] & 65536) {
      break label$4
     }
     if (HEAP32[$0 + 4 >> 2] & 1) {
      break label$4
     }
     $3 = HEAP32[$0 + 24 >> 2] + 1 | 0;
     break label$3;
    }
    $3 = __page_mapcount($0);
   }
   $4 = HEAP32[$0 + 28 >> 2];
   $5 = $0 + 12 | 0;
   $6 = HEAP32[$5 + 4 >> 2];
   HEAP32[$2 + 76 >> 2] = HEAP32[$5 >> 2];
   HEAP32[$2 + 80 >> 2] = $6;
   HEAP32[$2 + 72 >> 2] = $3;
   HEAP32[$2 + 68 >> 2] = $4;
   HEAP32[$2 + 64 >> 2] = $0;
   printk(26017, $2 - -64 | 0);
   label$5 : {
    if (!(HEAP32[$0 >> 2] & 65536)) {
     if (!(HEAP32[$0 + 4 >> 2] & 1)) {
      break label$5
     }
    }
    $3 = HEAP32[$0 + 4 >> 2];
    HEAP32[$2 + 48 >> 2] = HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) + 48 >> 2] + 1;
    printk(26072, $2 + 48 | 0);
   }
   printk(26097, 0);
   $3 = HEAP32[$0 >> 2];
   HEAP32[$2 + 36 >> 2] = $0;
   HEAP32[$2 + 32 >> 2] = $3;
   printk(26101, $2 + 32 | 0);
  }
  print_hex_dump($0);
  if ($1) {
   HEAP32[$2 >> 2] = $1;
   printk(26131, $2);
  }
  global$0 = $2 + 96 | 0;
 }
 
 function workingset_eviction($0) {
  var $1 = 0, $2 = 0;
  $1 = HEAP32[35610] + 1 | 0;
  HEAP32[35610] = $1;
  $2 = HEAP32[$0 + 4 >> 2];
  return (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] >>> 6 & 1 | (HEAP32[35586] | $1 >>> HEAP32[20648]) << 1) << 1 | 1;
 }
 
 function workingset_update_node($0) {
  $0 = $0 | 0;
  var $1 = 0;
  label$1 : {
   label$2 : {
    $1 = HEAPU8[$0 + 2 | 0];
    if (!$1) {
     break label$2
    }
    if (($1 | 0) != HEAPU8[$0 + 3 | 0]) {
     break label$2
    }
    $1 = HEAP32[$0 + 12 >> 2];
    $0 = $0 + 12 | 0;
    if (($1 | 0) != ($0 | 0)) {
     break label$1
    }
    list_lru_add($0);
    HEAP32[35623] = HEAP32[35623] + 1;
    HEAP32[20677] = HEAP32[20677] + 1;
    return;
   }
   $1 = HEAP32[$0 + 12 >> 2];
   $0 = $0 + 12 | 0;
   if (($1 | 0) == ($0 | 0)) {
    break label$1
   }
   list_lru_del($0);
   HEAP32[35623] = HEAP32[35623] + -1;
   HEAP32[20677] = HEAP32[20677] + -1;
  }
 }
 
 function list_lru_add($0) {
  var $1 = 0, $2 = 0;
  if (($0 | 0) != HEAP32[$0 >> 2]) {
   return
  }
  $1 = HEAP32[20647];
  $2 = HEAP32[$1 + 4 >> 2];
  HEAP32[$1 + 4 >> 2] = $0;
  HEAP32[$0 >> 2] = $1;
  HEAP32[$2 >> 2] = $0;
  HEAP32[$0 + 4 >> 2] = $2;
  HEAP32[$1 + 8 >> 2] = HEAP32[$1 + 8 >> 2] + 1;
  HEAP32[$1 + 12 >> 2] = HEAP32[$1 + 12 >> 2] + 1;
 }
 
 function list_lru_del($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = HEAP32[20647];
  $2 = HEAP32[$0 >> 2];
  if (($0 | 0) != ($2 | 0)) {
   $3 = HEAP32[$0 + 4 >> 2];
   HEAP32[$3 >> 2] = $2;
   HEAP32[$2 + 4 >> 2] = $3;
   HEAP32[$1 + 8 >> 2] = HEAP32[$1 + 8 >> 2] + -1;
   HEAP32[$1 + 12 >> 2] = HEAP32[$1 + 12 >> 2] + -1;
   HEAP32[$0 >> 2] = $0;
   HEAP32[$0 + 4 >> 2] = $0;
  }
 }
 
 function vma_interval_tree_iter_first($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = HEAP32[$0 >> 2];
  label$1 : {
   label$2 : {
    if (!$3) {
     break label$2
    }
    if (HEAPU32[$3 + 12 >> 2] < $1 >>> 0) {
     break label$2
    }
    if (HEAPU32[HEAP32[$0 + 4 >> 2] + 32 >> 2] > $2 >>> 0) {
     break label$2
    }
    $3 = $3 + -44 | 0;
    while (1) {
     $0 = $3;
     $4 = HEAP32[$0 + 52 >> 2];
     if ($4) {
      $3 = $4 + -44 | 0;
      if (HEAPU32[$4 + 12 >> 2] >= $1 >>> 0) {
       continue
      }
     }
     $3 = HEAP32[$0 + 76 >> 2];
     if ($3 >>> 0 > $2 >>> 0) {
      break label$2
     }
     if (($3 + (HEAP32[$0 + 4 >> 2] - HEAP32[$0 >> 2] >>> 16 | 0) | 0) + -1 >>> 0 >= $1 >>> 0) {
      break label$1
     }
     $0 = HEAP32[$0 + 48 >> 2];
     if (!$0) {
      break label$2
     }
     $3 = $0 + -44 | 0;
     if (HEAPU32[$0 + 12 >> 2] >= $1 >>> 0) {
      continue
     }
     break;
    };
   }
   $0 = 0;
  }
  return $0;
 }
 
 function vma_interval_tree_iter_next($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  $3 = HEAP32[$0 + 48 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     while (1) {
      if (HEAPU32[$3 + 12 >> 2] >= $1 >>> 0 ? $3 : 0) {
       break label$3
      }
      while (1) {
       $4 = HEAP32[$0 + 44 >> 2] & -4;
       if (!$4) {
        break label$2
       }
       $5 = $0 + 44 | 0;
       $0 = $4 + -44 | 0;
       $3 = HEAP32[$4 + 4 >> 2];
       if (($5 | 0) == ($3 | 0)) {
        continue
       }
       break;
      };
      $5 = HEAP32[$4 + 32 >> 2];
      if ($5 >>> 0 > $2 >>> 0) {
       break label$2
      }
      if (($5 + (HEAP32[$4 + -40 >> 2] - HEAP32[$0 >> 2] >>> 16 | 0) | 0) + -1 >>> 0 < $1 >>> 0) {
       continue
      }
      break;
     };
     return $0;
    }
    $3 = $3 + -44 | 0;
    while (1) {
     $0 = $3;
     $4 = HEAP32[$0 + 52 >> 2];
     if ($4) {
      $3 = $4 + -44 | 0;
      if (HEAPU32[$4 + 12 >> 2] >= $1 >>> 0) {
       continue
      }
     }
     $3 = HEAP32[$0 + 76 >> 2];
     if ($3 >>> 0 > $2 >>> 0) {
      break label$2
     }
     if (($3 + (HEAP32[$0 + 4 >> 2] - HEAP32[$0 >> 2] >>> 16 | 0) | 0) + -1 >>> 0 >= $1 >>> 0) {
      break label$1
     }
     $0 = HEAP32[$0 + 48 >> 2];
     if (!$0) {
      break label$2
     }
     $3 = $0 + -44 | 0;
     if (HEAPU32[$0 + 12 >> 2] >= $1 >>> 0) {
      continue
     }
     break;
    };
   }
   return 0;
  }
  return $0;
 }
 
 function anon_vma_interval_tree_iter_first($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = HEAP32[$0 >> 2];
  label$1 : {
   label$2 : {
    if (!$3) {
     break label$2
    }
    if (HEAPU32[$3 + 12 >> 2] < $1 >>> 0) {
     break label$2
    }
    if (HEAPU32[HEAP32[HEAP32[$0 + 4 >> 2] + -16 >> 2] + 76 >> 2] > $2 >>> 0) {
     break label$2
    }
    $3 = $3 + -16 | 0;
    while (1) {
     $0 = $3;
     $4 = HEAP32[$0 + 24 >> 2];
     if ($4) {
      $3 = $4 + -16 | 0;
      if (HEAPU32[$4 + 12 >> 2] >= $1 >>> 0) {
       continue
      }
     }
     $3 = HEAP32[$0 >> 2];
     $4 = HEAP32[$3 + 76 >> 2];
     if ($4 >>> 0 > $2 >>> 0) {
      break label$2
     }
     if (($4 + (HEAP32[$3 + 4 >> 2] - HEAP32[$3 >> 2] >>> 16 | 0) | 0) + -1 >>> 0 >= $1 >>> 0) {
      break label$1
     }
     $0 = HEAP32[$0 + 20 >> 2];
     if (!$0) {
      break label$2
     }
     $3 = $0 + -16 | 0;
     if (HEAPU32[$0 + 12 >> 2] >= $1 >>> 0) {
      continue
     }
     break;
    };
   }
   $0 = 0;
  }
  return $0;
 }
 
 function anon_vma_interval_tree_iter_next($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  $3 = HEAP32[$0 + 20 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     while (1) {
      if (HEAPU32[$3 + 12 >> 2] >= $1 >>> 0 ? $3 : 0) {
       break label$3
      }
      while (1) {
       $3 = HEAP32[$0 + 16 >> 2] & -4;
       if (!$3) {
        break label$2
       }
       $4 = $0 + 16 | 0;
       $0 = $3 + -16 | 0;
       $3 = HEAP32[$3 + 4 >> 2];
       if (($4 | 0) == ($3 | 0)) {
        continue
       }
       break;
      };
      $4 = HEAP32[$0 >> 2];
      $5 = HEAP32[$4 + 76 >> 2];
      if ($5 >>> 0 > $2 >>> 0) {
       break label$2
      }
      if (((HEAP32[$4 + 4 >> 2] - HEAP32[$4 >> 2] >>> 16 | 0) + $5 | 0) + -1 >>> 0 < $1 >>> 0) {
       continue
      }
      break;
     };
     return $0;
    }
    $3 = $3 + -16 | 0;
    while (1) {
     $0 = $3;
     $4 = HEAP32[$0 + 24 >> 2];
     if ($4) {
      $3 = $4 + -16 | 0;
      if (HEAPU32[$4 + 12 >> 2] >= $1 >>> 0) {
       continue
      }
     }
     $3 = HEAP32[$0 >> 2];
     $4 = HEAP32[$3 + 76 >> 2];
     if ($4 >>> 0 > $2 >>> 0) {
      break label$2
     }
     if (($4 + (HEAP32[$3 + 4 >> 2] - HEAP32[$3 >> 2] >>> 16 | 0) | 0) + -1 >>> 0 >= $1 >>> 0) {
      break label$1
     }
     $0 = HEAP32[$0 + 20 >> 2];
     if (!$0) {
      break label$2
     }
     $3 = $0 + -16 | 0;
     if (HEAPU32[$0 + 12 >> 2] >= $1 >>> 0) {
      continue
     }
     break;
    };
   }
   return 0;
  }
  return $0;
 }
 
 function congestion_wait() {
  var $0 = 0, $1 = 0;
  $0 = global$0 - 32 | 0;
  global$0 = $0;
  $1 = $0 + 20 | 0;
  HEAP32[$0 + 24 >> 2] = $1;
  HEAP32[$0 + 8 >> 2] = 0;
  HEAP32[$0 + 20 >> 2] = $1;
  HEAP32[$0 + 16 >> 2] = 76;
  HEAP32[$0 + 12 >> 2] = HEAP32[2];
  prepare_to_wait($0 + 8 | 0);
  io_schedule_timeout();
  finish_wait($0 + 8 | 0);
  global$0 = $0 + 32 | 0;
 }
 
 function wait_iff_congested() {
  var $0 = 0, $1 = 0;
  $0 = global$0 - 32 | 0;
  global$0 = $0;
  $1 = $0 + 20 | 0;
  HEAP32[$0 + 24 >> 2] = $1;
  HEAP32[$0 + 8 >> 2] = 0;
  HEAP32[$0 + 20 >> 2] = $1;
  HEAP32[$0 + 16 >> 2] = 76;
  HEAP32[$0 + 12 >> 2] = HEAP32[2];
  label$1 : {
   if (HEAP32[20652]) {
    prepare_to_wait($0 + 8 | 0);
    io_schedule_timeout();
    finish_wait($0 + 8 | 0);
    break label$1;
   }
   _cond_resched();
  }
  global$0 = $0 + 32 | 0;
 }
 
 function __next_zones_zonelist($0, $1, $2) {
  var $3 = 0;
  if ($2) {
   $2 = $0 + 4 | 0;
   while (1) {
    $0 = HEAP32[$2 >> 2];
    $3 = $2 + 8 | 0;
    $2 = $3;
    if ($0 >>> 0 > $1 >>> 0) {
     continue
    }
    break;
   };
   return $3 + -12 | 0;
  }
  $2 = $0 + 4 | 0;
  while (1) {
   $0 = HEAP32[$2 >> 2];
   $3 = $2 + 8 | 0;
   $2 = $3;
   if ($0 >>> 0 > $1 >>> 0) {
    continue
   }
   break;
  };
  return $3 + -12 | 0;
 }
 
 function lruvec_init() {
  memset(142384, 0, 64);
  HEAP32[35596] = 142384;
  HEAP32[35598] = 142392;
  HEAP32[35600] = 142400;
  HEAP32[35602] = 142408;
  HEAP32[35599] = 142392;
  HEAP32[35601] = 142400;
  HEAP32[35603] = 142408;
  HEAP32[35605] = 142416;
  HEAP32[35604] = 142416;
  HEAP32[35597] = 142384;
 }
 
 function page_mapped($0) {
  var $1 = 0;
  label$1 : {
   if (HEAP32[$0 + 4 >> 2] & 1 ? 0 : !(HEAP32[$0 >> 2] & 65536)) {
    break label$1
   }
   $1 = HEAP32[$0 + 4 >> 2];
   $0 = $1 & 1 ? $1 + -1 | 0 : $0;
   if (HEAP32[$0 + 48 >> 2] <= -1) {
    break label$1
   }
   return 1;
  }
  return HEAP32[$0 + 24 >> 2] > -1;
 }
 
 function page_anon_vma($0) {
  var $1 = 0;
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) + 12 >> 2];
  return ($0 & 3) == 1 ? $0 & -4 : 0;
 }
 
 function page_mapping($0) {
  var $1 = 0, $2 = 0;
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = $1 & 1 ? $1 + -1 | 0 : $0;
  $1 = HEAP32[$0 + 4 >> 2];
  label$1 : {
   if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 512) {
    break label$1
   }
   $0 = HEAP32[$0 + 12 >> 2];
   if ($0 & 1) {
    break label$1
   }
   $2 = $0 & -4;
  }
  return $2;
 }
 
 function __page_mapcount($0) {
  var $1 = 0, $2 = 0;
  $1 = HEAP32[$0 + 24 >> 2];
  $2 = HEAP32[$0 + 4 >> 2];
  if (!(HEAP8[($2 & 1 ? $2 + -1 | 0 : $0) + 12 | 0] & 1)) {
   return $1 + 1 | 0
  }
  $2 = $1;
  $1 = HEAP32[$0 + 4 >> 2];
  return ($2 + HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) + 48 >> 2] | 0) + 2 | 0;
 }
 
 function shrink_slab($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0;
  $6 = global$0 - 48 | 0;
  global$0 = $6;
  if (__down_read_trylock(16528)) {
   $8 = HEAP32[4135];
   if (($8 | 0) != 16540) {
    $17 = $6 + 32 | 0;
    $13 = $6 + 24 | 0;
    $15 = $6 + 28 | 0;
    while (1) {
     HEAP32[$17 >> 2] = 0;
     HEAP32[$13 >> 2] = 0;
     HEAP32[$13 + 4 >> 2] = 0;
     HEAP32[$6 + 16 >> 2] = $0;
     HEAP32[$6 + 20 >> 2] = $1;
     $4 = HEAP32[$8 + -4 >> 2];
     $3 = HEAP32[$8 + -12 >> 2];
     $14 = $8 + -20 | 0;
     $7 = FUNCTION_TABLE[HEAP32[$14 >> 2]]($14, $6 + 16 | 0) | 0;
     label$5 : {
      if (!($7 ? ($7 | 0) != -2 : 0)) {
       $4 = $7;
       break label$5;
      }
      $11 = $3 ? $3 : 128;
      $16 = (0 - ($4 & 1) & $1) << 2;
      $3 = $16 + HEAP32[$8 + 8 >> 2] | 0;
      $12 = HEAP32[$3 >> 2];
      HEAP32[$3 >> 2] = 0;
      label$8 : {
       label$9 : {
        $4 = HEAP32[$8 + -8 >> 2];
        label$10 : {
         if ($4) {
          $5 = $7 >> $2;
          $3 = $5 >> 31 << 2 | $5 >>> 30;
          $5 = $5 << 2;
          HEAP32[$6 + 40 >> 2] = $5;
          HEAP32[$6 + 44 >> 2] = $3;
          if (!$3 & $5 >>> 0 > 4294967295 | $3 >>> 0 > 0) {
           break label$9
          }
          $4 = ($5 >>> 0) / ($4 >>> 0) | 0;
          $5 = 0;
          break label$10;
         }
         $3 = ($7 | 0) / 2 | 0;
         $4 = $3;
         $5 = $3 >> 31;
        }
        HEAP32[$6 + 40 >> 2] = $4;
        HEAP32[$6 + 44 >> 2] = $5;
        break label$8;
       }
       __div64_32($6 + 40 | 0, $4);
       $4 = HEAP32[$6 + 40 >> 2];
       $5 = HEAP32[$6 + 44 >> 2];
      }
      $3 = ($7 | 0) < ($11 | 0) ? $7 : $11;
      $9 = $3;
      $10 = $3;
      $3 = $3 >> 31;
      $9 = ($3 | 0) == ($5 | 0) & $4 >>> 0 > $9 >>> 0 | $5 >>> 0 > $3 >>> 0;
      $4 = $9 ? $4 : $10;
      $3 = $9 ? $5 : $3;
      $5 = $3;
      HEAP32[$6 + 40 >> 2] = $4;
      HEAP32[$6 + 44 >> 2] = $3;
      $3 = $4 + $12 | 0;
      label$12 : {
       if (($3 | 0) > -1) {
        $12 = $3;
        break label$12;
       }
       $4 = HEAP32[$8 + -16 >> 2];
       HEAP32[$6 + 4 >> 2] = $3;
       HEAP32[$6 >> 2] = $4;
       printk(26227, $6);
       $4 = HEAP32[$6 + 40 >> 2];
       $5 = HEAP32[$6 + 44 >> 2];
       $3 = $7;
      }
      $9 = ($7 | 0) / 4 | 0;
      $10 = $9;
      $9 = $9 >> 31;
      if (!(($9 | 0) == ($5 | 0) & $4 >>> 0 >= $10 >>> 0 | $5 >>> 0 > $9 >>> 0)) {
       $4 = ($7 | 0) / 2 | 0;
       $3 = ($3 | 0) < ($4 | 0) ? $3 : $4;
      }
      $4 = $7 << 1;
      $3 = ($3 | 0) > ($4 | 0) ? $4 : $3;
      label$15 : {
       if (($3 | 0) < ($11 | 0)) {
        $5 = 0;
        $4 = 0;
        if (($3 | 0) < ($7 | 0)) {
         break label$15
        }
       }
       $9 = $8 + -16 | 0;
       $4 = 0;
       $5 = 0;
       while (1) {
        $10 = ($11 | 0) < ($3 | 0) ? $11 : $3;
        HEAP32[$15 >> 2] = $10;
        HEAP32[$13 >> 2] = $10;
        $10 = FUNCTION_TABLE[HEAP32[$9 >> 2]]($14, $6 + 16 | 0) | 0;
        if (($10 | 0) == -1) {
         break label$15
        }
        $4 = $4 + $10 | 0;
        $10 = HEAP32[$15 >> 2];
        $5 = $10 + $5 | 0;
        _cond_resched();
        $3 = $3 - $10 | 0;
        if (($3 | 0) >= ($11 | 0)) {
         continue
        }
        if (($3 | 0) >= ($7 | 0)) {
         continue
        }
        break;
       };
      }
      $3 = HEAP32[$8 + 8 >> 2] + $16 | 0;
      $7 = ($12 | 0) < ($5 | 0) ? 0 : $12 - $5 | 0;
      if (($7 | 0) >= 1) {
       HEAP32[$3 >> 2] = $7 + HEAP32[$3 >> 2]
      }
     }
     $18 = (($4 | 0) == -2 ? 0 : $4) + $18 | 0;
     if (HEAP32[4133] == 16532) {
      $8 = HEAP32[$8 >> 2];
      if (($8 | 0) != 16540) {
       continue
      }
     }
     break;
    };
   }
   __up_read(16528);
  }
  _cond_resched();
  global$0 = $6 + 48 | 0;
 }
 
 function __remove_mapping($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $2 = global$0 - 32 | 0;
  global$0 = $2;
  $4 = HEAP32[$1 + 4 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 1) {
      if ((page_mapping($1) | 0) != ($0 | 0)) {
       break label$3
      }
      $4 = HEAPU8[82796];
      HEAP8[82796] = 0;
      $3 = HEAPU8[82796];
      HEAP8[82796] = 0;
      label$5 : {
       if (HEAP32[$1 + 28 >> 2] == 2) {
        $5 = $1 + 28 | 0;
        HEAP32[$5 >> 2] = 0;
        HEAP8[82796] = $3 & 1;
        $3 = HEAP32[$1 + 4 >> 2];
        if (HEAP32[($3 & 1 ? $3 + -1 | 0 : $1) >> 2] & 8) {
         break label$5
        }
        $3 = HEAP32[HEAP32[$0 + 52 >> 2] + 40 >> 2];
        $7 = $1;
        $5 = HEAP32[$1 + 4 >> 2];
        $6 = 0;
        label$7 : {
         if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 524288) {
          break label$7
         }
         $6 = 0;
         if (HEAP32[$0 + 56 >> 2] & 16) {
          break label$7
         }
         $6 = workingset_eviction($1);
        }
        __delete_from_page_cache($7, $6);
        $0 = 1;
        HEAP8[82796] = $4 & 1;
        if (!$3) {
         break label$1
        }
        FUNCTION_TABLE[$3]($1);
        break label$1;
       }
       HEAP8[82796] = $3 & 1;
       break label$2;
      }
      HEAP32[$5 >> 2] = 2;
      break label$2;
     }
     HEAP32[$2 + 8 >> 2] = 26288;
     HEAP32[$2 + 4 >> 2] = 891;
     HEAP32[$2 >> 2] = 26218;
     printk(26180, $2);
     abort();
    }
    HEAP32[$2 + 24 >> 2] = 26288;
    HEAP32[$2 + 20 >> 2] = 892;
    HEAP32[$2 + 16 >> 2] = 26218;
    printk(26180, $2 + 16 | 0);
    abort();
   }
   $0 = 0;
   HEAP8[82796] = $4 & 1;
  }
  global$0 = $2 + 32 | 0;
  return $0;
 }
 
 function putback_lru_page($0) {
  var $1 = 0;
  lru_cache_add($0);
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = $1 & 1 ? $1 + -1 | 0 : $0;
  $1 = HEAP32[$0 + 28 >> 2] + -1 | 0;
  HEAP32[$0 + 28 >> 2] = $1;
  if ($1) {
   return
  }
  __put_page($0);
 }
 
 function shrink_page_list($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0;
  $7 = global$0 + -64 | 0;
  global$0 = $7;
  HEAP32[$7 + 20 >> 2] = $7 + 16;
  HEAP32[$7 + 16 >> 2] = $7 + 16;
  HEAP32[$7 + 12 >> 2] = $7 + 8;
  HEAP32[$7 + 8 >> 2] = $7 + 8;
  _cond_resched();
  label$1 : {
   label$2 : {
    if (($0 | 0) != HEAP32[$0 >> 2]) {
     $25 = $0 + 4 | 0;
     $12 = $2 + 20 | 0;
     $15 = $2 + 12 | 0;
     $14 = $2 + 16 | 0;
     $16 = $1 + 1176 | 0;
     break label$2;
    }
    break label$1;
   }
   while (1) {
    _cond_resched();
    $6 = HEAP32[$25 >> 2];
    $1 = HEAP32[$6 >> 2];
    $4 = HEAP32[$6 + 4 >> 2];
    HEAP32[$1 + 4 >> 2] = $4;
    HEAP32[$4 >> 2] = $1;
    HEAP32[$6 >> 2] = 256;
    HEAP32[$6 + 4 >> 2] = 512;
    $4 = HEAP32[$6 >> 2];
    $1 = $6 + -4 | 0;
    $5 = $4 & 1 ? $4 + -1 | 0 : $1;
    $4 = HEAP32[$5 >> 2];
    HEAP32[$7 + 24 >> 2] = $4;
    label$5 : {
     label$6 : {
      if (HEAP32[$7 + 24 >> 2] & 1) {
       break label$6
      }
      HEAP32[$5 >> 2] = $4 | 1;
      if ($4 & 1) {
       break label$6
      }
      HEAP32[$12 >> 2] = HEAP32[$12 >> 2] + 1;
      $4 = page_mapping($1);
      label$7 : {
       label$8 : {
        if (HEAP32[$4 + 56 >> 2] & 8 ? $4 : 0) {
         break label$8
        }
        $4 = HEAP32[$6 >> 2];
        if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 2097152) {
         break label$8
        }
        if (!(HEAPU8[$15 | 0] & 2)) {
         if (page_mapped($1)) {
          break label$7
         }
        }
        label$11 : {
         if (!page_mapped($1)) {
          break label$11
         }
         $4 = HEAP32[$6 >> 2];
         if (HEAP8[($4 & 1 ? $4 + -1 | 0 : $1) + 12 | 0] & 1) {
          $4 = HEAP32[$6 >> 2];
          if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 524288)) {
           break label$11
          }
         }
         HEAP32[$12 >> 2] = HEAP32[$12 >> 2] + 1;
        }
        $26 = HEAP32[$14 >> 2];
        label$13 : {
         label$14 : {
          $4 = HEAP32[$6 >> 2];
          if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 524288)) {
           $4 = HEAP32[$6 >> 2];
           if (!(HEAP8[($4 & 1 ? $4 + -1 | 0 : $1) + 12 | 0] & 1)) {
            break label$14
           }
           $4 = HEAP32[$6 >> 2];
           if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 524288) {
            break label$14
           }
          }
          HEAP8[$7 + 6 | 0] = 0;
          HEAP8[$7 + 7 | 0] = 0;
          break label$13;
         }
         $4 = HEAP32[$6 >> 2];
         HEAP8[$7 + 7 | 0] = HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] >>> 3 & 1;
         $4 = HEAP32[$6 >> 2];
         HEAP8[$7 + 6 | 0] = HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] >>> 15 & 1;
         if (!(HEAPU8[$1 + 1 | 0] & 96)) {
          break label$13
         }
         $4 = page_mapping($1);
         if (!$4) {
          break label$13
         }
         $4 = HEAP32[HEAP32[$4 + 52 >> 2] + 68 >> 2];
         if (!$4) {
          break label$13
         }
         FUNCTION_TABLE[$4]($1, $7 + 7 | 0, $7 + 6 | 0);
        }
        $8 = HEAPU8[$7 + 6 | 0];
        $9 = HEAPU8[$7 + 7 | 0];
        $4 = page_mapping($1);
        $5 = HEAPU8[$7 + 6 | 0];
        $10 = !$8 & ($9 | 0) != 0;
        $8 = ($8 | $9) != 0;
        label$16 : {
         label$17 : {
          label$18 : {
           if (!$4) {
            break label$18
           }
           if (!($5 | HEAPU8[$7 + 7 | 0])) {
            break label$18
           }
           $5 = HEAP32[$4 >> 2];
           label$19 : {
            if ($5) {
             $5 = HEAP32[HEAP32[$5 + 20 >> 2] + 108 >> 2];
             break label$19;
            }
            $5 = 16184;
           }
           $9 = HEAP32[$5 + 56 >> 2];
           $11 = HEAP32[$9 + 16 >> 2];
           label$21 : {
            if ($11) {
             if (FUNCTION_TABLE[$11](HEAP32[$9 + 20 >> 2], 1)) {
              break label$17
             }
             break label$21;
            }
            if (HEAP32[HEAP32[$5 + 136 >> 2] >> 2] & 1) {
             break label$17
            }
           }
           $5 = HEAPU8[$7 + 6 | 0];
          }
          if (!$5) {
           break label$16
          }
          $5 = HEAP32[$6 >> 2];
          if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 262144)) {
           break label$16
          }
         }
         $17 = $17 + 1 | 0;
        }
        $18 = $10 + $18 | 0;
        $19 = $8 + $19 | 0;
        $5 = HEAP32[$6 >> 2];
        if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 32768)) {
         $9 = page_referenced($1, 1, HEAP32[$2 + 8 >> 2], $7 + 60 | 0);
         $5 = HEAP32[$6 >> 2];
         $11 = $5 & 1 ? $5 + -1 | 0 : $1;
         $10 = HEAP32[$11 >> 2];
         HEAP32[$7 + 24 >> 2] = $10;
         $8 = 0;
         $5 = 0;
         if (HEAP32[$7 + 24 >> 2] & 2) {
          HEAP32[$11 >> 2] = $10 & -3;
          $5 = $10 >>> 1 & 1;
         }
         label$24 : {
          $10 = HEAP32[$7 + 60 >> 2];
          label$25 : {
           if ($10 & 8192) {
            break label$25
           }
           if ($9) {
            break label$24
           }
           if ($5) {
            $8 = 1;
            $5 = HEAP32[$6 >> 2];
            if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 524288)) {
             break label$25
            }
           }
           $8 = 0;
          }
          label$28 : {
           label$29 : {
            label$30 : {
             label$31 : {
              $5 = HEAP32[$6 >> 2];
              if (HEAP8[($5 & 1 ? $5 + -1 | 0 : $1) + 12 | 0] & 1) {
               $5 = HEAP32[$6 >> 2];
               if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 524288) {
                break label$31
               }
              }
              if (page_mapped($1)) {
               if (!try_to_unmap($1)) {
                break label$30
               }
              }
              $5 = HEAP32[$6 >> 2];
              if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 8)) {
               break label$28
              }
              $5 = HEAP32[$6 >> 2];
              if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 524288) {
               break label$29
              }
              label$34 : {
               if (!(HEAPU8[HEAP32[2] + 14 | 0] & 2)) {
                break label$34
               }
               $5 = HEAP32[$6 >> 2];
               if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 262144)) {
                break label$34
               }
               if (HEAP32[$16 >> 2] & 2) {
                break label$29
               }
              }
              HEAP32[35640] = HEAP32[35640] + 1;
              HEAP32[20694] = HEAP32[20694] + 1;
              $4 = HEAP32[$6 >> 2];
              $4 = $4 & 1 ? $4 + -1 | 0 : $1;
              HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 262144;
              break label$8;
             }
             if (HEAPU8[$14 | 0] & 64) {
              break label$8
             }
             break label$7;
            }
            $20 = $20 + 1 | 0;
            break label$8;
           }
           if (!($26 & 128)) {
            break label$7
           }
           if ($8) {
            break label$7
           }
           if (!(HEAP8[$15 | 0] & 1)) {
            break label$7
           }
           $5 = HEAP32[$6 >> 2];
           $8 = HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) + 28 >> 2];
           $5 = HEAP32[$1 >> 2] & 24576;
           if (($8 - (($5 | 0) != 0) | 0) != 2) {
            break label$7
           }
           label$35 : {
            label$36 : {
             label$37 : {
              if ($4) {
               if (!HEAP32[HEAP32[$4 + 52 >> 2] >> 2]) {
                break label$8
               }
               if (HEAPU8[HEAP32[2] + 14 | 0] & 128) {
                break label$35
               }
               $5 = HEAP32[$4 >> 2];
               if (!$5) {
                break label$37
               }
               $8 = HEAP32[HEAP32[$5 + 20 >> 2] + 108 >> 2];
               break label$36;
              }
              if (!$5) {
               break label$7
              }
              $4 = HEAP32[$6 >> 2];
              $4 = $4 & 1 ? $4 + -1 | 0 : $1;
              HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -9;
              HEAP32[$7 >> 2] = 26333;
              printk(26305, $7);
              $4 = 0;
              break label$28;
             }
             $8 = 16184;
            }
            $9 = HEAP32[$8 + 56 >> 2];
            $10 = HEAP32[$9 + 16 >> 2];
            label$39 : {
             if ($10) {
              if (FUNCTION_TABLE[$10](HEAP32[$9 + 20 >> 2], 1)) {
               break label$39
              }
              break label$35;
             }
             if (!(HEAP32[HEAP32[$8 + 136 >> 2] >> 2] & 1)) {
              break label$35
             }
            }
            if (HEAP32[HEAP32[2] + 808 >> 2] != (($5 ? HEAP32[HEAP32[$5 + 20 >> 2] + 108 >> 2] : 16184) | 0)) {
             break label$7
            }
           }
           if (!clear_page_dirty_for_io($1)) {
            break label$28
           }
           $5 = HEAP32[6593];
           $8 = $7 + 48 | 0;
           HEAP32[$8 >> 2] = HEAP32[6592];
           HEAP32[$8 + 4 >> 2] = $5;
           $5 = HEAP32[6591];
           $8 = $7 + 40 | 0;
           HEAP32[$8 >> 2] = HEAP32[6590];
           HEAP32[$8 + 4 >> 2] = $5;
           $5 = HEAP32[6589];
           $8 = $7 + 32 | 0;
           HEAP32[$8 >> 2] = HEAP32[6588];
           HEAP32[$8 + 4 >> 2] = $5;
           $5 = HEAP32[6587];
           HEAP32[$7 + 24 >> 2] = HEAP32[6586];
           HEAP32[$7 + 28 >> 2] = $5;
           $5 = HEAP32[$6 >> 2];
           $5 = $5 & 1 ? $5 + -1 | 0 : $1;
           HEAP32[$5 >> 2] = HEAP32[$5 >> 2] | 262144;
           $5 = FUNCTION_TABLE[HEAP32[HEAP32[$4 + 52 >> 2] >> 2]]($1, $7 + 24 | 0) | 0;
           label$43 : {
            if (($5 | 0) > -1) {
             if (($5 | 0) != 524288) {
              break label$43
             }
             $4 = HEAP32[$6 >> 2];
             $4 = $4 & 1 ? $4 + -1 | 0 : $1;
             HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -262145;
             break label$8;
            }
            $8 = HEAP32[$6 >> 2];
            $9 = $8 & 1 ? $8 + -1 | 0 : $1;
            $8 = HEAP32[$9 >> 2];
            HEAP32[$7 + 60 >> 2] = $8;
            label$45 : {
             if (!(HEAP32[$7 + 60 >> 2] & 1)) {
              HEAP32[$9 >> 2] = $8 | 1;
              if (!($8 & 1)) {
               break label$45
              }
             }
             __lock_page($1);
            }
            if ((page_mapping($1) | 0) == ($4 | 0)) {
             errseq_set($4 + 60 | 0, $5);
             HEAP32[$4 + 56 >> 2] = HEAP32[$4 + 56 >> 2] | (($5 | 0) == -28 ? 2 : 1);
            }
            unlock_page($1);
           }
           $4 = HEAP32[$6 >> 2];
           if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 32768)) {
            $4 = HEAP32[$6 >> 2];
            $4 = $4 & 1 ? $4 + -1 | 0 : $1;
            HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -262145;
           }
           HEAP32[35639] = HEAP32[35639] + 1;
           HEAP32[20693] = HEAP32[20693] + 1;
           $4 = HEAP32[$6 >> 2];
           if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 32768) {
            break label$6
           }
           $4 = HEAP32[$6 >> 2];
           if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 8) {
            break label$6
           }
           $4 = HEAP32[$6 >> 2];
           $5 = $4 & 1 ? $4 + -1 | 0 : $1;
           $4 = HEAP32[$5 >> 2];
           HEAP32[$7 + 24 >> 2] = $4;
           if (HEAP32[$7 + 24 >> 2] & 1) {
            break label$6
           }
           HEAP32[$5 >> 2] = $4 | 1;
           if ($4 & 1) {
            break label$6
           }
           $4 = HEAP32[$6 >> 2];
           if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 8) {
            break label$7
           }
           $4 = HEAP32[$6 >> 2];
           if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 32768) {
            break label$7
           }
           $4 = page_mapping($1);
          }
          label$49 : {
           label$50 : {
            label$51 : {
             if (!(HEAPU8[$1 + 1 | 0] & 96)) {
              break label$51
             }
             if (!try_to_release_page($1, HEAP32[$14 >> 2])) {
              break label$8
             }
             if ($4) {
              break label$51
             }
             $5 = HEAP32[$6 >> 2];
             if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) + 28 >> 2] != 1) {
              break label$51
             }
             unlock_page($1);
             $1 = HEAP32[$6 + 24 >> 2] + -1 | 0;
             HEAP32[$6 + 24 >> 2] = $1;
             if (!$1) {
              break label$50
             }
             $13 = $13 + 1 | 0;
             break label$5;
            }
            label$52 : {
             label$53 : {
              $5 = HEAP32[$6 >> 2];
              if (!(HEAP8[($5 & 1 ? $5 + -1 | 0 : $1) + 12 | 0] & 1)) {
               break label$53
              }
              $5 = HEAP32[$6 >> 2];
              if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $1) >> 2] & 524288) {
               break label$53
              }
              $4 = HEAPU8[82796];
              HEAP8[82796] = 0;
              if (HEAP32[$6 + 24 >> 2] != 1) {
               break label$49
              }
              $5 = $6 + 24 | 0;
              HEAP32[$5 >> 2] = 0;
              HEAP8[82796] = $4 & 1;
              $4 = HEAP32[$6 >> 2];
              if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 8)) {
               break label$52
              }
              HEAP32[$5 >> 2] = 1;
              break label$7;
             }
             if (!$4) {
              break label$7
             }
             if (!__remove_mapping($4, $1)) {
              break label$7
             }
            }
            $4 = HEAP32[$6 >> 2];
            $1 = $4 & 1 ? $4 + -1 | 0 : $1;
            HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -2;
           }
           $1 = HEAP32[$7 + 8 >> 2];
           HEAP32[$1 + 4 >> 2] = $6;
           HEAP32[$6 >> 2] = $1;
           HEAP32[$7 + 8 >> 2] = $6;
           HEAP32[$6 + 4 >> 2] = $7 + 8;
           $13 = $13 + 1 | 0;
           break label$5;
          }
          HEAP8[82796] = $4 & 1;
          break label$7;
         }
         $4 = HEAP32[$6 >> 2];
         if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 524288) {
          break label$8
         }
         $4 = HEAP32[$6 >> 2];
         $4 = $4 & 1 ? $4 + -1 | 0 : $1;
         HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 2;
         if (($9 | 0) > 1) {
          break label$8
         }
         if ($10 & 4 | $5) {
          break label$8
         }
         $21 = $21 + 1 | 0;
         break label$7;
        }
        label$54 : {
         if (!(HEAPU8[HEAP32[2] + 14 | 0] & 2)) {
          break label$54
         }
         $4 = HEAP32[$6 >> 2];
         if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 262144)) {
          break label$54
         }
         if (!(HEAP32[$16 >> 2] & 4)) {
          break label$54
         }
         $22 = $22 + 1 | 0;
         break label$8;
        }
        $4 = HEAP32[$6 >> 2];
        $4 = $4 & 1 ? $4 + -1 | 0 : $1;
        HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 262144;
        $23 = $23 + 1 | 0;
       }
       $4 = HEAP32[$6 >> 2];
       if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 2097152) {
        break label$7
       }
       $4 = HEAP32[$6 >> 2];
       $4 = $4 & 1 ? $4 + -1 | 0 : $1;
       HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 32;
       $24 = $24 + 1 | 0;
      }
      unlock_page($1);
     }
     $1 = HEAP32[$7 + 16 >> 2];
     HEAP32[$1 + 4 >> 2] = $6;
     HEAP32[$6 >> 2] = $1;
     HEAP32[$7 + 16 >> 2] = $6;
     HEAP32[$6 + 4 >> 2] = $7 + 16;
    }
    if (HEAP32[$0 >> 2] != ($0 | 0)) {
     continue
    }
    break;
   };
  }
  free_unref_page_list($7 + 8 | 0);
  $1 = HEAP32[$7 + 16 >> 2];
  if (($1 | 0) != ($7 + 16 | 0)) {
   $2 = HEAP32[$7 + 20 >> 2];
   HEAP32[$1 + 4 >> 2] = $0;
   $6 = HEAP32[$0 >> 2];
   HEAP32[$0 >> 2] = $1;
   HEAP32[$6 + 4 >> 2] = $2;
   HEAP32[$2 >> 2] = $6;
  }
  if ($3) {
   HEAP32[$3 + 8 >> 2] = $17;
   HEAP32[$3 >> 2] = $19;
   HEAP32[$3 + 28 >> 2] = $20;
   HEAP32[$3 + 24 >> 2] = $21;
   HEAP32[$3 + 20 >> 2] = $24;
   HEAP32[$3 + 16 >> 2] = $22;
   HEAP32[$3 + 12 >> 2] = $23;
   HEAP32[$3 + 4 >> 2] = $18;
  }
  global$0 = $7 - -64 | 0;
  return $13;
 }
 
 function __isolate_lru_page($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  $3 = -22;
  $2 = HEAP32[$0 + 4 >> 2];
  label$1 : {
   if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 16)) {
    break label$1
   }
   $2 = HEAP32[$0 + 4 >> 2];
   $2 = HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2];
   if ($2 & 1048576 ? !($1 & 8) : 0) {
    break label$1
   }
   label$3 : {
    if (!($1 & 4)) {
     break label$3
    }
    $3 = -16;
    $2 = $0 + 4 | 0;
    $4 = HEAP32[$2 >> 2];
    if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $0) >> 2] & 32768) {
     break label$1
    }
    $2 = HEAP32[$2 >> 2];
    if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 8)) {
     break label$3
    }
    $2 = HEAP32[$0 + 4 >> 2];
    $4 = $2 & 1 ? $2 + -1 | 0 : $0;
    $2 = HEAP32[$4 >> 2];
    HEAP32[$5 + 12 >> 2] = $2;
    if (HEAP32[$5 + 12 >> 2] & 1) {
     break label$1
    }
    HEAP32[$4 >> 2] = $2 | 1;
    if ($2 & 1) {
     break label$1
    }
    $2 = page_mapping($0);
    if ($2) {
     $2 = HEAP32[HEAP32[$2 + 52 >> 2] + 48 >> 2];
     unlock_page($0);
     if ($2) {
      break label$3
     }
     break label$1;
    }
    unlock_page($0);
   }
   if ($1 & 2) {
    $3 = -16;
    if (page_mapped($0)) {
     break label$1
    }
   }
   $1 = HEAP32[$0 + 28 >> 2];
   if ($1) {
    HEAP32[$0 + 28 >> 2] = $1 + 1;
    $1 = HEAP32[$0 + 4 >> 2];
    $0 = $1 & 1 ? $1 + -1 | 0 : $0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -17;
    $3 = 0;
    break label$1;
   }
   $3 = -16;
  }
  global$0 = $5 + 16 | 0;
  return $3;
 }
 
 function isolate_lru_page($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  if (HEAP32[$0 + 4 >> 2] & 1) {
   ___ratelimit(16548, 26163)
  }
  $3 = -16;
  $1 = $0 + 4 | 0;
  $2 = HEAP32[$1 >> 2];
  if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 16) {
   $2 = HEAP32[$0 >> 2];
   HEAP8[82796] = 0;
   $1 = HEAP32[$1 >> 2];
   if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 16) {
    $6 = HEAP32[Math_imul($2 >>> 31 | 0, 516) + 141296 >> 2] + 1112 | 0;
    $2 = 4;
    $3 = $0 + 4 | 0;
    $1 = HEAP32[$3 >> 2];
    if (!(HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 1048576)) {
     $2 = $0 + 4 | 0;
     $1 = HEAP32[$2 >> 2];
     $2 = HEAP32[$2 >> 2];
     $2 = (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] >>> 5 & 1 | HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] >>> 18 & 2) ^ 2;
    }
    $1 = HEAP32[$3 >> 2];
    $1 = $1 & 1 ? $1 + -1 | 0 : $0;
    HEAP32[$1 + 28 >> 2] = HEAP32[$1 + 28 >> 2] + 1;
    $1 = HEAP32[$3 >> 2];
    $4 = $1 & 1 ? $1 + -1 | 0 : $0;
    HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -17;
    $4 = $0 + 8 | 0;
    $7 = HEAP32[$4 >> 2];
    HEAP32[$7 >> 2] = $1;
    $6 = $6 + -1112 | 0;
    $2 = $2 << 2;
    $5 = ($6 + $2 | 0) + 1184 | 0;
    HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + -1;
    $5 = $2 + 82672 | 0;
    HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + -1;
    HEAP32[$1 + 4 >> 2] = $7;
    $0 = ($2 + ($6 + Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) | 0) | 0) + 472 | 0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
    HEAP32[$3 >> 2] = 256;
    HEAP32[$4 >> 2] = 512;
    $0 = $2 + 82628 | 0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
    $3 = 0;
   }
   HEAP8[82796] = 1;
  }
  return $3;
 }
 
 function try_to_free_pages($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0;
  $3 = global$0 - 80 | 0;
  global$0 = $3;
  HEAP32[$3 + 8 >> 2] = 0;
  HEAP8[$3 + 12 | 0] = !HEAP32[20702] | 6;
  HEAP32[$3 + 4 >> 2] = 0;
  HEAP32[$3 >> 2] = 32;
  HEAP8[$3 + 14 | 0] = 12;
  HEAP8[$3 + 13 | 0] = $0;
  HEAP8[$3 + 15 | 0] = 1024 >>> ($1 & 15) & 1;
  $0 = HEAP32[2];
  $4 = HEAP32[$0 + 12 >> 2];
  label$1 : {
   if (!($4 & 524288)) {
    $4 = $4 & 262144 ? $1 & -129 : $1;
    break label$1;
   }
   $4 = $1 & -193;
  }
  HEAP32[$3 + 52 >> 2] = 0;
  $1 = $3 + 44 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $1 = $3 + 36 | 0;
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
  $13 = $3 + 16 | 0;
  HEAP32[$13 >> 2] = $4;
  HEAP32[$3 + 28 >> 2] = 0;
  HEAP32[$3 + 32 >> 2] = 0;
  HEAP32[$3 + 20 >> 2] = 0;
  HEAP32[$3 + 24 >> 2] = 0;
  label$3 : {
   label$4 : {
    if (HEAPU8[$0 + 14 | 0] & 32) {
     break label$4
    }
    if (HEAP8[$0 + 761 | 0] & 1 ? HEAP32[HEAP32[$0 + 4 >> 2] >> 2] & 4 : 0) {
     break label$4
    }
    $1 = 142304;
    $5 = 1024 >>> ($4 & 15) & 1;
    label$6 : {
     if (HEAPU32[$1 + 4 >> 2] <= $5 >>> 0) {
      $0 = HEAP32[35576];
      if (!$0) {
       break label$4
      }
      break label$6;
     }
     $1 = __next_zones_zonelist(142304, $5, 0);
     $0 = HEAP32[$1 >> 2];
     if (!$0) {
      break label$4
     }
    }
    while (1) {
     label$9 : {
      $2 = HEAP32[$0 + 24 >> 2];
      if (($0 - $2 | 0) > 0) {
       $0 = $1 + 8 | 0;
       if (HEAPU32[$1 + 12 >> 2] <= $5 >>> 0) {
        $1 = $0;
        break label$9;
       }
       $1 = __next_zones_zonelist($0, $5, 0);
       break label$9;
      }
      if (HEAP32[$2 + 1104 >> 2] > 15) {
       break label$4
      }
      if (!HEAP32[$2 + 40 >> 2]) {
       break label$4
      }
      if (HEAP32[$2 + 484 >> 2] == (0 - HEAP32[$2 + 480 >> 2] | 0)) {
       break label$4
      }
      $1 = HEAP32[$2 >> 2];
      if (!$1) {
       break label$4
      }
      if (HEAPU32[$2 + 468 >> 2] > $1 >>> 1 >>> 0) {
       break label$4
      }
      label$12 : {
       label$13 : {
        label$14 : {
         label$15 : {
          label$16 : {
           label$17 : {
            label$18 : {
             $5 = $2 + 1076 | 0;
             label$19 : {
              label$20 : {
               label$21 : {
                if (($5 | 0) != HEAP32[$2 + 1076 >> 2]) {
                 HEAP32[$2 + 1100 >> 2] = 0;
                 $1 = 1;
                 __wake_up($5, 1, 0);
                 $4 = !($4 & 128);
                 if (HEAP32[$2 + 1104 >> 2] > 15) {
                  break label$20
                 }
                 if (HEAP32[$2 + 40 >> 2]) {
                  break label$21
                 }
                 break label$20;
                }
                $4 = !($4 & 128);
               }
               $1 = 1;
               if (HEAP32[$2 + 484 >> 2] == (0 - HEAP32[$2 + 480 >> 2] | 0)) {
                break label$20
               }
               $6 = HEAP32[$2 >> 2];
               if (!$6) {
                break label$20
               }
               if (HEAPU32[$2 + 468 >> 2] > $6 >>> 1 >>> 0) {
                break label$20
               }
               if (HEAP32[$5 >> 2] != ($5 | 0)) {
                HEAP32[$2 + 1100 >> 2] = 0;
                __wake_up($2 + 1076 | 0, 1, 0);
                if ($4) {
                 break label$19
                }
                break label$17;
               }
               $1 = 0;
              }
              if (!$4) {
               break label$18
              }
              if ($1) {
               break label$12
              }
             }
             init_wait_entry($3 + 56 | 0);
             $0 = $2 + 1084 | 0;
             $4 = prepare_to_wait_event($0, $3 + 56 | 0, 1);
             $6 = $2 + 1104 | 0;
             if (HEAP32[$6 >> 2] <= 15) {
              break label$16
             }
             break label$13;
            }
            if ($1) {
             break label$12
            }
           }
           init_wait_entry($3 + 56 | 0);
           $0 = $0 + 24 | 0;
           $1 = prepare_to_wait_event(HEAP32[$0 >> 2] + 1084 | 0, $3 + 56 | 0, 258);
           $4 = $2 + 1104 | 0;
           if (HEAP32[$4 >> 2] <= 15) {
            break label$15
           }
           break label$14;
          }
          $10 = $2 + 1076 | 0;
          $1 = 250;
          $7 = $2 + 40 | 0;
          $11 = $2 + 480 | 0;
          $12 = $2 + 484 | 0;
          $8 = $2 + 468 | 0;
          $9 = $2 + 1100 | 0;
          while (1) {
           if (!HEAP32[$7 >> 2]) {
            break label$13
           }
           if (HEAP32[$12 >> 2] == (0 - HEAP32[$11 >> 2] | 0)) {
            break label$13
           }
           $14 = HEAP32[$2 >> 2];
           if (!$14) {
            break label$13
           }
           if (HEAPU32[$8 >> 2] > $14 >>> 1 >>> 0) {
            break label$13
           }
           if (HEAP32[$5 >> 2] != ($5 | 0)) {
            HEAP32[$9 >> 2] = 0;
            __wake_up($10, 1, 0);
           }
           if (!$1) {
            break label$13
           }
           if ($4) {
            break label$12
           }
           $1 = schedule_timeout($1);
           $4 = prepare_to_wait_event($0, $3 + 56 | 0, 1);
           if (HEAP32[$6 >> 2] < 16) {
            continue
           }
           break;
          };
          break label$13;
         }
         $6 = $2 + 1076 | 0;
         $10 = $2 + 40 | 0;
         $7 = $2 + 480 | 0;
         $11 = $2 + 484 | 0;
         $12 = $2 + 468 | 0;
         $8 = $2 + 1100 | 0;
         while (1) {
          if (!HEAP32[$10 >> 2]) {
           break label$14
          }
          if (HEAP32[$11 >> 2] == (0 - HEAP32[$7 >> 2] | 0)) {
           break label$14
          }
          $9 = HEAP32[$2 >> 2];
          if (!$9) {
           break label$14
          }
          if (HEAPU32[$12 >> 2] > $9 >>> 1 >>> 0) {
           break label$14
          }
          if (HEAP32[$5 >> 2] != ($5 | 0)) {
           HEAP32[$8 >> 2] = 0;
           __wake_up($6, 1, 0);
          }
          if ($1) {
           break label$12
          }
          schedule();
          $1 = prepare_to_wait_event(HEAP32[$0 >> 2] + 1084 | 0, $3 + 56 | 0, 258);
          if (HEAP32[$4 >> 2] < 16) {
           continue
          }
          break;
         };
        }
        finish_wait($3 + 56 | 0);
        break label$12;
       }
       finish_wait($3 + 56 | 0);
      }
      $0 = HEAP32[2];
      if (!(HEAP32[HEAP32[$0 + 4 >> 2] >> 2] & 4)) {
       break label$4
      }
      $1 = 1;
      if (HEAP8[$0 + 761 | 0] & 1) {
       break label$3
      }
      break label$4;
     }
     $0 = HEAP32[$1 >> 2];
     if ($0) {
      continue
     }
     break;
    };
   }
   $6 = $3 + 14 | 0;
   $11 = HEAPU8[$6 | 0];
   $12 = $3 + 20 | 0;
   $2 = $3 + 15 | 0;
   $10 = $3 + 24 | 0;
   $5 = $3 + 12 | 0;
   label$28 : {
    while (1) {
     $8 = HEAP32[$13 >> 2];
     while (1) {
      HEAP32[$12 >> 2] = 0;
      $0 = HEAP8[$2 | 0];
      label$31 : {
       label$32 : {
        label$33 : {
         $4 = HEAP32[$3 + 4 >> 2];
         if ($4) {
          break label$33
         }
         $1 = 142304;
         if (HEAPU32[35577] > $0 >>> 0) {
          break label$33
         }
         $0 = HEAP32[35576];
         if (!$0) {
          break label$31
         }
         break label$32;
        }
        $1 = __next_zones_zonelist(142304, $0, $4);
        $0 = HEAP32[$1 >> 2];
        if (!$0) {
         break label$31
        }
       }
       $4 = 0;
       while (1) {
        $0 = HEAP32[$0 + 24 >> 2];
        if (($4 | 0) != ($0 | 0)) {
         shrink_node($0, $3);
         $4 = $0;
        }
        $0 = $1 + 8 | 0;
        $7 = HEAP8[$2 | 0];
        label$36 : {
         $9 = HEAP32[$3 + 4 >> 2];
         if ($9) {
          break label$36
         }
         if (HEAPU32[$1 + 12 >> 2] > $7 >>> 0) {
          break label$36
         }
         $1 = $0;
         $0 = HEAP32[$0 >> 2];
         if ($0) {
          continue
         }
         break label$31;
        }
        $1 = __next_zones_zonelist($0, $7, $9);
        $0 = HEAP32[$1 >> 2];
        if ($0) {
         continue
        }
        break;
       };
      }
      HEAP32[$13 >> 2] = $8;
      label$37 : {
       if (HEAPU32[$10 >> 2] >= HEAPU32[$3 >> 2]) {
        break label$37
       }
       $0 = HEAPU8[$5 | 0];
       if ($0 & 64) {
        break label$37
       }
       $1 = HEAP8[$6 | 0];
       if (($1 | 0) <= 9) {
        HEAP8[$5 | 0] = $0 | 1
       }
       $0 = $1 + -1 | 0;
       HEAP8[$6 | 0] = $0;
       if ($0 << 24 >> 24 > -1) {
        continue
       }
      }
      break;
     };
     $0 = HEAP8[$2 | 0];
     label$39 : {
      label$40 : {
       label$41 : {
        $4 = HEAP32[$3 + 4 >> 2];
        if ($4) {
         break label$41
        }
        $1 = 142304;
        if (HEAPU32[35577] > $0 >>> 0) {
         break label$41
        }
        $0 = HEAP32[35576];
        if (!$0) {
         break label$39
        }
        break label$40;
       }
       $1 = __next_zones_zonelist(142304, $0, $4);
       $0 = HEAP32[$1 >> 2];
       if (!$0) {
        break label$39
       }
      }
      $4 = 0;
      while (1) {
       $0 = HEAP32[$0 + 24 >> 2];
       if (($4 | 0) != ($0 | 0)) {
        HEAP32[$0 + 1172 >> 2] = HEAP32[20679];
        $4 = $0;
       }
       $0 = $1 + 8 | 0;
       $7 = HEAP8[$2 | 0];
       label$44 : {
        $8 = HEAP32[$3 + 4 >> 2];
        if ($8) {
         break label$44
        }
        if (HEAPU32[$1 + 12 >> 2] > $7 >>> 0) {
         break label$44
        }
        $1 = $0;
        $0 = HEAP32[$0 >> 2];
        if ($0) {
         continue
        }
        break label$39;
       }
       $1 = __next_zones_zonelist($0, $7, $8);
       $0 = HEAP32[$1 >> 2];
       if ($0) {
        continue
       }
       break;
      };
     }
     $1 = HEAP32[$10 >> 2];
     if ($1) {
      break label$3
     }
     $0 = HEAPU8[$5 | 0];
     if ($0 & 64) {
      break label$28
     }
     if ($0 & 16) {
      HEAP8[$6 | 0] = $11;
      HEAP8[$5 | 0] = $0 & 231 | 8;
      continue;
     }
     break;
    };
    $1 = 0;
    break label$3;
   }
   $1 = 1;
  }
  global$0 = $3 + 80 | 0;
  return $1;
 }
 
 function shrink_node($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0;
  $4 = global$0 - 96 | 0;
  global$0 = $4;
  $14 = HEAP32[HEAP32[2] + 804 >> 2];
  $16 = $1 + 28 | 0;
  memset($16, 0, 28);
  $22 = HEAP32[$1 + 24 >> 2];
  $10 = HEAP8[$1 + 14 | 0];
  $23 = HEAP32[$1 >> 2];
  $6 = HEAP8[$1 + 15 | 0];
  $8 = $6 + 1 | 0;
  label$1 : {
   if (($8 | 0) <= 1) {
    $5 = 1 - $6 | 0;
    $2 = (Math_imul($8, 516) + $0 | 0) + 472 | 0;
    while (1) {
     $2 = $2 + 516 | 0;
     $5 = $5 + -1 | 0;
     if ($5) {
      continue
     }
     break;
    };
    HEAP32[$4 + 32 >> 2] = 0;
    $5 = 1 - $6 | 0;
    $2 = (Math_imul($8, 516) + $0 | 0) + 476 | 0;
    while (1) {
     $2 = $2 + 516 | 0;
     $5 = $5 + -1 | 0;
     if ($5) {
      continue
     }
     break;
    };
    $3 = 1 - $6 | 0;
    $2 = (Math_imul($8, 516) + $0 | 0) + 480 | 0;
    HEAP32[$4 + 36 >> 2] = 0;
    $5 = HEAP32[20670];
    while (1) {
     if (HEAP32[$2 + -440 >> 2]) {
      $9 = HEAP32[$2 >> 2];
      $5 = $5 - ($9 >>> 0 < $5 >>> 0 ? $9 : $5) | 0;
     }
     $2 = $2 + 516 | 0;
     $3 = $3 + -1 | 0;
     if ($3) {
      continue
     }
     break;
    };
    HEAP32[$4 + 40 >> 2] = $5 >>> $10;
    $3 = 1 - $6 | 0;
    $2 = (Math_imul($8, 516) + $0 | 0) + 484 | 0;
    $5 = HEAP32[20671];
    while (1) {
     if (HEAP32[$2 + -444 >> 2]) {
      $6 = HEAP32[$2 >> 2];
      $5 = $5 - ($6 >>> 0 < $5 >>> 0 ? $6 : $5) | 0;
     }
     $2 = $2 + 516 | 0;
     $3 = $3 + -1 | 0;
     if ($3) {
      continue
     }
     break;
    };
    break label$1;
   }
   HEAP32[$4 + 32 >> 2] = 0;
   HEAP32[$4 + 36 >> 2] = 0;
   HEAP32[$4 + 40 >> 2] = HEAP32[20670] >>> $10;
   $5 = HEAP32[20671];
  }
  HEAP32[$4 + 44 >> 2] = $5 >>> $10;
  memcpy($4, $4 + 32 | 0, 20);
  $17 = HEAPU8[HEAP32[2] + 14 | 0] & 2 ? 0 : HEAPU8[$1 + 14 | 0] == 12;
  label$12 : {
   $10 = HEAP32[$4 + 32 >> 2];
   if (!(HEAP32[$4 + 40 >> 2] | ($10 | HEAP32[$4 + 44 >> 2]))) {
    break label$12
   }
   $12 = $0 + 1112 | 0;
   $24 = $4 + 32 | 12;
   $25 = $4 + 32 | 4;
   $26 = $0 + 556 | 0;
   $27 = $1 + 15 | 0;
   $28 = $0 + 1172 | 0;
   $29 = $1 + 12 | 0;
   $13 = $0 + 1216 | 0;
   $18 = $0 + 1164 | 0;
   $19 = $0 + 1156 | 0;
   $30 = $1 + 8 | 0;
   label$13 : while (1) {
    $6 = 0;
    $2 = $10 ? 0 : 1;
    while (1) {
     if (!$2) {
      $7 = $10 >>> 0 < 32 ? $10 : 32;
      HEAP32[($4 + 32 | 0) + ($6 << 2) >> 2] = $10 - $7;
      label$19 : {
       label$20 : {
        label$21 : {
         if (($6 | 2) == 3) {
          $9 = 0;
          $3 = $6 | 1;
          if (($3 | 0) != 3) {
           break label$19
          }
          $2 = ($3 | 0) == 3;
          $3 = $2 << 1;
          $11 = $3 | 1;
          $3 = HEAP32[($3 << 2) + 82672 >> 2];
          $8 = HEAP8[$27 | 0];
          if (($8 + 1 | 0) > 1) {
           break label$21
          }
          $20 = $2 << 3;
          $31 = $20 + 988 | 0;
          $2 = Math_imul($8, 516) + $26 | 0;
          $5 = $2;
          $8 = 1 - $8 | 0;
          $9 = $8;
          while (1) {
           if (HEAP32[$5 >> 2]) {
            $21 = HEAP32[($5 + $31 | 0) + -556 >> 2];
            $3 = $3 - ($21 >>> 0 < $3 >>> 0 ? $21 : $3) | 0;
           }
           $5 = $5 + 516 | 0;
           $9 = $9 + -1 | 0;
           if ($9) {
            continue
           }
           break;
          };
          $9 = $20 | 992;
          $5 = HEAP32[($11 << 2) + 82672 >> 2];
          while (1) {
           if (HEAP32[$2 >> 2]) {
            $11 = HEAP32[($2 + $9 | 0) + -556 >> 2];
            $5 = $5 - ($11 >>> 0 < $5 >>> 0 ? $11 : $5) | 0;
           }
           $2 = $2 + 516 | 0;
           $8 = $8 + -1 | 0;
           if ($8) {
            continue
           }
           break;
          };
          break label$20;
         }
         $9 = shrink_inactive_list($7, $12, $1, $6);
         break label$19;
        }
        $5 = HEAP32[($11 << 2) + 82672 >> 2];
       }
       $9 = 0;
       $2 = 0;
       label$27 : {
        if (HEAP32[$28 >> 2] != HEAP32[20679]) {
         break label$27
        }
        $2 = $3 + $5 >>> 14 | 0;
        if ($2) {
         $2 = int_sqrt(Math_imul($2, 10));
         break label$27;
        }
        $2 = 1;
       }
       if (Math_imul($2, $3) >>> 0 >= $5 >>> 0) {
        break label$19
       }
       HEAP32[$4 + 84 >> 2] = $4 + 80;
       HEAP32[$4 + 80 >> 2] = $4 + 80;
       HEAP32[$4 + 76 >> 2] = $4 + 72;
       HEAP32[$4 + 72 >> 2] = $4 + 72;
       HEAP32[$4 + 68 >> 2] = $4 - -64;
       HEAP32[$4 + 64 >> 2] = $4 - -64;
       lru_add_drain_cpu();
       $3 = HEAPU8[$29 | 0];
       HEAP8[82796] = 0;
       $5 = isolate_lru_pages($7, $12, $4 + 80 | 0, $4 + 92 | 0, $1, ($3 ^ -1) & 2, $6);
       HEAP32[$13 >> 2] = $5 + HEAP32[$13 >> 2];
       HEAP32[$18 >> 2] = $5 + HEAP32[$18 >> 2];
       HEAP8[82796] = 1;
       HEAP32[20676] = $5 + HEAP32[20676];
       $8 = 0;
       label$29 : {
        if (HEAP32[$4 + 80 >> 2] == ($4 + 80 | 0)) {
         break label$29
        }
        while (1) {
         _cond_resched();
         $3 = HEAP32[$4 + 84 >> 2];
         $2 = HEAP32[$3 >> 2];
         $7 = HEAP32[$3 + 4 >> 2];
         HEAP32[$2 + 4 >> 2] = $7;
         HEAP32[$7 >> 2] = $2;
         HEAP32[$3 >> 2] = 256;
         HEAP32[$3 + 4 >> 2] = 512;
         label$31 : {
          $2 = $3 + -4 | 0;
          $7 = page_mapping($2);
          if (HEAP32[$7 + 56 >> 2] & 8 ? $7 : 0) {
           break label$31
          }
          $7 = HEAP32[$3 >> 2];
          if (HEAP32[($7 & 1 ? $7 + -1 | 0 : $2) >> 2] & 2097152) {
           break label$31
          }
          label$33 : {
           label$34 : {
            if (!page_referenced($2, 0, HEAP32[$30 >> 2], $4 + 88 | 0)) {
             break label$34
            }
            $8 = $8 + 1 | 0;
            if (!(HEAPU8[$4 + 88 | 0] & 4)) {
             break label$34
            }
            $7 = HEAP32[$3 >> 2];
            $11 = $4 + 72 | 0;
            if (!(HEAP32[($7 & 1 ? $7 + -1 | 0 : $2) >> 2] & 524288)) {
             break label$33
            }
           }
           $7 = HEAP32[$3 >> 2];
           $7 = $7 & 1 ? $7 + -1 | 0 : $2;
           HEAP32[$7 >> 2] = HEAP32[$7 >> 2] & -33;
           $7 = HEAP32[$3 >> 2];
           $2 = $7 & 1 ? $7 + -1 | 0 : $2;
           HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 64;
           $11 = $4 - -64 | 0;
          }
          $2 = $11;
          $7 = HEAP32[$2 >> 2];
          HEAP32[$7 + 4 >> 2] = $3;
          HEAP32[$2 >> 2] = $3;
          HEAP32[$3 >> 2] = $7;
          HEAP32[$3 + 4 >> 2] = $2;
          if (HEAP32[$4 + 80 >> 2] != ($4 + 80 | 0)) {
           continue
          }
          break label$29;
         }
         lru_cache_add($2);
         $3 = HEAP32[$3 >> 2];
         $3 = $3 & 1 ? $3 + -1 | 0 : $2;
         $2 = HEAP32[$3 + 28 >> 2] + -1 | 0;
         HEAP32[$3 + 28 >> 2] = $2;
         if (!$2) {
          __put_page($3)
         }
         if (HEAP32[$4 + 80 >> 2] != ($4 + 80 | 0)) {
          continue
         }
         break;
        };
       }
       HEAP8[82796] = 0;
       HEAP32[$19 >> 2] = HEAP32[$19 >> 2] + $8;
       move_active_pages_to_lru($12, $4 + 72 | 0, $4 + 80 | 0, $6);
       move_active_pages_to_lru($12, $4 - -64 | 0, $4 + 80 | 0, $6 + -1 | 0);
       HEAP32[$13 >> 2] = HEAP32[$13 >> 2] - $5;
       HEAP8[82796] = 1;
       HEAP32[20676] = HEAP32[20676] - $5;
       free_unref_page_list($4 + 80 | 0);
      }
      $15 = $9 + $15 | 0;
      $2 = 1;
      continue;
     }
     label$36 : {
      $6 = $6 + 1 | 0;
      if (($6 | 0) != 4) {
       $10 = HEAP32[($4 + 32 | 0) + ($6 << 2) >> 2];
       if (!$10) {
        break label$36
       }
       $2 = 0;
       continue;
      }
      _cond_resched();
      if (!(($15 >>> 0 < $23 >>> 0 | $17) & 1)) {
       $3 = HEAP32[$4 + 40 >> 2];
       $5 = $3 + HEAP32[$4 + 44 >> 2] | 0;
       if (!$5) {
        break label$12
       }
       $2 = HEAP32[$4 + 32 >> 2];
       $6 = $2 + HEAP32[$4 + 36 >> 2] | 0;
       if (!$6) {
        break label$12
       }
       label$39 : {
        if ($5 >>> 0 > $6 >>> 0) {
         HEAP32[$4 + 32 >> 2] = 0;
         $5 = (Math_imul($6, 100) >>> 0) / ((HEAP32[$4 >> 2] + HEAP32[$4 + 4 >> 2] | 0) + 1 >>> 0) | 0;
         $8 = 2;
         $2 = $25;
         break label$39;
        }
        $8 = 0;
        HEAP32[$4 + 40 >> 2] = 0;
        $5 = (Math_imul($5, 100) >>> 0) / ((HEAP32[$4 + 8 >> 2] + HEAP32[$4 + 12 >> 2] | 0) + 1 >>> 0) | 0;
        $3 = $2;
        $2 = $24;
       }
       HEAP32[$2 >> 2] = 0;
       $6 = $8 << 2;
       $8 = HEAP32[($6 | $4) >> 2];
       $5 = 100 - $5 | 0;
       $2 = (Math_imul($8, $5) >>> 0) / 100 | 0;
       $3 = $8 - $3 | 0;
       HEAP32[($6 | $4 + 32) >> 2] = $2 - ($2 >>> 0 < $3 >>> 0 ? $2 : $3);
       $3 = $6 | 4;
       $2 = $3 | $4 + 32;
       $8 = $2;
       $9 = $5;
       $5 = HEAP32[($4 | $3) >> 2];
       $3 = (Math_imul($9, $5) >>> 0) / 100 | 0;
       $2 = $5 - HEAP32[$2 >> 2] | 0;
       HEAP32[$8 >> 2] = $3 - ($3 >>> 0 < $2 >>> 0 ? $3 : $2);
       $17 = 1;
      }
      $10 = HEAP32[$4 + 32 >> 2];
      if (HEAP32[$4 + 40 >> 2] | ($10 | HEAP32[$4 + 44 >> 2])) {
       continue label$13
      }
      break label$12;
     }
     $2 = 1;
     continue;
    };
   };
  }
  $3 = $1 + 24 | 0;
  HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + $15;
  shrink_slab(HEAP32[$1 + 16 >> 2], HEAP32[$0 + 1072 >> 2], HEAP8[$1 + 14 | 0]);
  label$41 : {
   if ($14) {
    $2 = HEAP32[$14 >> 2];
    HEAP32[$14 >> 2] = 0;
    $5 = $2 + HEAP32[$3 >> 2] | 0;
    HEAP32[$3 >> 2] = $5;
    break label$41;
   }
   $5 = HEAP32[$3 >> 2];
  }
  label$43 : {
   if (!(HEAPU8[HEAP32[2] + 14 | 0] & 2)) {
    break label$43
   }
   $3 = HEAP32[$1 + 40 >> 2];
   label$44 : {
    if (!$3) {
     break label$44
    }
    if (($3 | 0) != HEAP32[$1 + 52 >> 2]) {
     break label$44
    }
    HEAP32[$0 + 1176 >> 2] = HEAP32[$0 + 1176 >> 2] | 4;
   }
   $3 = HEAP32[$16 >> 2];
   label$45 : {
    if (!$3) {
     break label$45
    }
    if (($3 | 0) != HEAP32[$1 + 36 >> 2]) {
     break label$45
    }
    HEAP32[$0 + 1176 >> 2] = HEAP32[$0 + 1176 >> 2] | 1;
   }
   if (HEAP32[$1 + 32 >> 2] == HEAP32[$1 + 48 >> 2]) {
    HEAP32[$0 + 1176 >> 2] = HEAP32[$0 + 1176 >> 2] | 2
   }
   if (!HEAP32[$1 + 44 >> 2]) {
    break label$43
   }
   congestion_wait();
  }
  label$47 : {
   if (HEAPU8[$1 + 12 | 0] & 32) {
    break label$47
   }
   $1 = HEAP32[2];
   $3 = HEAP32[$1 + 12 >> 2];
   if ($3 & 131072) {
    break label$47
   }
   label$48 : {
    if (!($3 & 1048576)) {
     break label$48
    }
    $1 = HEAP32[$1 + 808 >> 2];
    if (!$1) {
     break label$48
    }
    $3 = HEAP32[$1 + 56 >> 2];
    $2 = HEAP32[$3 + 16 >> 2];
    if ($2) {
     if (FUNCTION_TABLE[$2](HEAP32[$3 + 20 >> 2], 1)) {
      break label$48
     }
     break label$47;
    }
    if (!(HEAP32[HEAP32[$1 + 136 >> 2] >> 2] & 1)) {
     break label$47
    }
   }
   if (!(HEAP32[$0 + 1176 >> 2] & 1)) {
    break label$47
   }
   wait_iff_congested();
  }
  if (($5 | 0) != ($22 | 0)) {
   HEAP32[$0 + 1104 >> 2] = 0
  }
  global$0 = $4 + 96 | 0;
 }
 
 function wakeup_kswapd($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  label$1 : {
   if (!HEAP32[$0 + 40 >> 2]) {
    break label$1
   }
   $0 = HEAP32[$0 + 24 >> 2];
   $3 = HEAP32[$0 + 1096 >> 2];
   HEAP32[$0 + 1096 >> 2] = ($3 | 0) > ($1 | 0) ? $3 : $1;
   $3 = HEAP32[$0 + 1100 >> 2];
   HEAP32[$0 + 1100 >> 2] = ($3 | 0) == 2 ? $2 : $3 >>> 0 > $2 >>> 0 ? $3 : $2;
   $5 = $0 + 1076 | 0;
   if (($5 | 0) == HEAP32[$0 + 1076 >> 2]) {
    break label$1
   }
   if (($2 | 0) < 0) {
    break label$1
   }
   if (HEAP32[$0 + 1104 >> 2] > 15) {
    break label$1
   }
   $3 = -1;
   $4 = -1;
   while (1) {
    if (HEAP32[$0 + 40 >> 2]) {
     $4 = HEAP32[$0 + 8 >> 2];
     if (zone_watermark_ok_safe($0, $1, $4, $2)) {
      break label$1
     }
    }
    $0 = $0 + 516 | 0;
    $3 = $3 + 1 | 0;
    if (($3 | 0) < ($2 | 0)) {
     continue
    }
    break;
   };
   if (($4 | 0) == -1) {
    break label$1
   }
   __wake_up($5, 1, 0);
  }
 }
 
 function page_evictable($0) {
  var $1 = 0, $2 = 0;
  $1 = page_mapping($0);
  label$1 : {
   if ($1) {
    $2 = 0;
    if (HEAP32[$1 + 56 >> 2] & 8) {
     break label$1
    }
   }
   $1 = HEAP32[$0 + 4 >> 2];
   $2 = (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] >>> 21 ^ -1) & 1;
  }
  return $2;
 }
 
 function shrink_inactive_list($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $5 = global$0 - 48 | 0;
  global$0 = $5;
  HEAP32[$5 + 44 >> 2] = $5 + 40;
  HEAP32[$5 + 40 >> 2] = $5 + 40;
  $4 = $5 + 24 | 0;
  HEAP32[$4 >> 2] = 0;
  HEAP32[$4 + 4 >> 2] = 0;
  $4 = $5 + 16 | 0;
  HEAP32[$4 >> 2] = 0;
  HEAP32[$4 + 4 >> 2] = 0;
  $4 = $5 + 8 | 0;
  HEAP32[$4 >> 2] = 0;
  HEAP32[$4 + 4 >> 2] = 0;
  HEAP32[$5 >> 2] = 0;
  HEAP32[$5 + 4 >> 2] = 0;
  $9 = $1 + -1112 | 0;
  $4 = HEAP32[2];
  $8 = $3 | 1;
  $6 = ($8 | 0) == 3;
  $7 = ($6 << 3) + 82672 | 0;
  $11 = ($6 ? 32 : 28) + 82672 | 0;
  label$1 : {
   label$2 : {
    label$3 : {
     while (1) {
      if (HEAPU8[$4 + 14 | 0] & 2) {
       break label$3
      }
      $4 = HEAP32[$7 >> 2];
      if (HEAPU32[$11 >> 2] <= ((HEAP32[$2 + 16 >> 2] & 192) == 192 ? $4 >>> 3 | 0 : $4) >>> 0) {
       break label$3
      }
      if ($12) {
       break label$2
      }
      $4 = 26;
      if ($4) {
       while (1) {
        HEAP32[HEAP32[2] >> 2] = 2;
        $4 = schedule_timeout($4);
        if ($4) {
         continue
        }
        break;
       }
      }
      $12 = 1;
      $4 = HEAP32[2];
      if (HEAP32[HEAP32[$4 + 4 >> 2] >> 2] & 4) {
       $10 = HEAP32[$4 + 760 >> 2] >>> 8 & 1
      } else {
       $10 = 0
      }
      if (!$10) {
       continue
      }
      break;
     };
     $4 = 32;
     break label$1;
    }
    lru_add_drain_cpu();
    $4 = 0;
    HEAP8[82796] = 0;
    $0 = isolate_lru_pages($0, $1, $5 + 40 | 0, $5 + 36 | 0, $2, (HEAPU8[$2 + 12 | 0] ^ -1) & 2, $3);
    $7 = ($8 | 0) == 3 ? 32 : 28;
    $3 = ($7 + $9 | 0) + 1184 | 0;
    HEAP32[$3 >> 2] = $0 + HEAP32[$3 >> 2];
    $6 = (($6 << 2) + $1 | 0) + 48 | 0;
    HEAP32[$6 >> 2] = $0 + HEAP32[$6 >> 2];
    HEAP8[82796] = 1;
    $6 = $7 + 82672 | 0;
    HEAP32[$6 >> 2] = $0 + HEAP32[$6 >> 2];
    if (!$0) {
     break label$1
    }
    $4 = shrink_page_list($5 + 40 | 0, $9, $2, $5);
    HEAP8[82796] = 0;
    putback_inactive_pages($1, $5 + 40 | 0);
    HEAP32[$3 >> 2] = HEAP32[$3 >> 2] - $0;
    HEAP8[82796] = 1;
    HEAP32[$6 >> 2] = HEAP32[$6 >> 2] - $0;
    free_unref_page_list($5 + 40 | 0);
    $1 = HEAP32[$5 + 4 >> 2];
    if (($1 | 0) == ($0 | 0)) {
     wakeup_flusher_threads();
     $1 = HEAP32[$5 + 4 >> 2];
    }
    HEAP32[$2 + 28 >> 2] = HEAP32[$2 + 28 >> 2] + HEAP32[$5 >> 2];
    $3 = $2 + 36 | 0;
    HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + HEAP32[$5 + 8 >> 2];
    $3 = $2 + 32 | 0;
    HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + $1;
    $1 = $2 + 40 | 0;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + HEAP32[$5 + 12 >> 2];
    $1 = $2 + 44 | 0;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + HEAP32[$5 + 16 >> 2];
    $1 = $2 + 52 | 0;
    HEAP32[$1 >> 2] = $0 + HEAP32[$1 >> 2];
    if (($8 | 0) != 3) {
     break label$1
    }
    $1 = $2 + 48 | 0;
    HEAP32[$1 >> 2] = $0 + HEAP32[$1 >> 2];
    break label$1;
   }
   $4 = 0;
  }
  global$0 = $5 + 48 | 0;
  return $4;
 }
 
 function isolate_lru_pages($0, $1, $2, $3, $4, $5, $6) {
  var $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $7 = global$0 - 48 | 0;
  global$0 = $7;
  HEAP32[$7 + 40 >> 2] = 0;
  HEAP32[$7 + 44 >> 2] = 0;
  HEAP32[$7 + 32 >> 2] = 0;
  HEAP32[$7 + 36 >> 2] = 0;
  $10 = ($6 << 3) + $1 | 0;
  HEAP32[$7 + 28 >> 2] = $7 + 24;
  HEAP32[$7 + 24 >> 2] = $7 + 24;
  label$1 : {
   label$2 : {
    if ($0) {
     $15 = $10 + 4 | 0;
     $16 = $4 + 15 | 0;
     while (1) {
      if (($10 | 0) == HEAP32[$10 >> 2]) {
       break label$2
      }
      $4 = HEAP32[$15 >> 2];
      $8 = $4 + -4 | 0;
      $9 = HEAP32[$8 >> 2] >>> 31 | 0;
      label$5 : {
       if ($9 >>> 0 > HEAP8[$16 | 0] >>> 0) {
        $8 = HEAP32[$4 >> 2];
        $11 = HEAP32[$4 + 4 >> 2];
        HEAP32[$8 + 4 >> 2] = $11;
        HEAP32[$11 >> 2] = $8;
        $8 = HEAP32[$7 + 24 >> 2];
        HEAP32[$8 + 4 >> 2] = $4;
        HEAP32[$4 >> 2] = $8;
        $8 = $7 + 32 | $9 << 2;
        HEAP32[$8 >> 2] = HEAP32[$8 >> 2] + 1;
        HEAP32[$7 + 24 >> 2] = $4;
        HEAP32[$4 + 4 >> 2] = $7 + 24;
        break label$5;
       }
       $13 = $13 + 1 | 0;
       $9 = __isolate_lru_page($8, $5);
       if (($9 | 0) != -16) {
        if ($9) {
         break label$1
        }
        $9 = HEAP32[$4 >> 2];
        $11 = HEAP32[$4 + 4 >> 2];
        HEAP32[$9 + 4 >> 2] = $11;
        HEAP32[$11 >> 2] = $9;
        $9 = HEAP32[$2 >> 2];
        HEAP32[$9 + 4 >> 2] = $4;
        HEAP32[$4 >> 2] = $9;
        HEAP32[$2 >> 2] = $4;
        HEAP32[$4 + 4 >> 2] = $2;
        $4 = $7 + 40 | HEAP32[$8 >> 2] >>> 29 & 4;
        HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + 1;
        $12 = $12 + 1 | 0;
        break label$5;
       }
       $8 = HEAP32[$4 >> 2];
       $9 = HEAP32[$4 + 4 >> 2];
       HEAP32[$8 + 4 >> 2] = $9;
       HEAP32[$9 >> 2] = $8;
       $8 = HEAP32[$10 >> 2];
       HEAP32[$8 + 4 >> 2] = $4;
       HEAP32[$4 >> 2] = $8;
       HEAP32[$10 >> 2] = $4;
       HEAP32[$4 + 4 >> 2] = $10;
      }
      $14 = $14 + 1 | 0;
      if ($13 >>> 0 >= $0 >>> 0) {
       break label$2
      }
      if ($12 >>> 0 < $0 >>> 0) {
       continue
      }
      break;
     };
     break label$2;
    }
   }
   label$8 : {
    if (HEAP32[$7 + 24 >> 2] == ($7 + 24 | 0)) {
     break label$8
    }
    $0 = HEAP32[$7 + 24 >> 2];
    if (($0 | 0) == ($7 + 24 | 0)) {
     break label$8
    }
    $2 = HEAP32[$7 + 28 >> 2];
    HEAP32[$0 + 4 >> 2] = $10;
    $4 = HEAP32[$10 >> 2];
    HEAP32[$10 >> 2] = $0;
    HEAP32[$4 + 4 >> 2] = $2;
    HEAP32[$2 >> 2] = $4;
   }
   HEAP32[$3 >> 2] = $14;
   $4 = $1 + -1112 | 0;
   $1 = $6 << 2;
   $0 = ($4 + $1 | 0) + 1184 | 0;
   $1 = $1 + 82672 | 0;
   $5 = $6 + 1 | 0;
   $6 = $5 << 2;
   $2 = $6 + 82624 | 0;
   $3 = HEAP32[$7 + 40 >> 2];
   if ($3) {
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] - $3;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] - $3;
    $6 = ($4 + $6 | 0) + 468 | 0;
    HEAP32[$6 >> 2] = HEAP32[$6 >> 2] - $3;
    HEAP32[$2 >> 2] = HEAP32[$2 >> 2] - $3;
   }
   $3 = HEAP32[$7 + 44 >> 2];
   if ($3) {
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] - $3;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] - $3;
    $0 = ($4 + ($5 << 2) | 0) + 984 | 0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] - $3;
    HEAP32[$2 >> 2] = HEAP32[$2 >> 2] - $3;
   }
   global$0 = $7 + 48 | 0;
   return $12;
  }
  HEAP32[$7 + 8 >> 2] = 26376;
  HEAP32[$7 + 4 >> 2] = 1716;
  HEAP32[$7 >> 2] = 26218;
  printk(26180, $7);
  abort();
 }
 
 function move_active_pages_to_lru($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  if (($1 | 0) != HEAP32[$1 >> 2]) {
   $7 = $0 + -1112 | 0;
   $6 = ($7 + ($3 << 3) | 0) + 1112 | 0;
   $0 = $3 << 2;
   $8 = ($0 + $7 | 0) + 1184 | 0;
   $9 = $0 + 82672 | 0;
   $11 = $3 + 1 << 2;
   $10 = $11 + 82624 | 0;
   $12 = $1 + 4 | 0;
   while (1) {
    $0 = HEAP32[$12 >> 2];
    $4 = HEAP32[$0 >> 2];
    $3 = $0 + -4 | 0;
    $5 = $4 & 1 ? $4 + -1 | 0 : $3;
    HEAP32[$5 >> 2] = HEAP32[$5 >> 2] | 16;
    HEAP32[$8 >> 2] = HEAP32[$8 >> 2] + 1;
    HEAP32[$9 >> 2] = HEAP32[$9 >> 2] + 1;
    $5 = ((Math_imul(HEAP32[$3 >> 2] >>> 31 | 0, 516) + $7 | 0) + $11 | 0) + 468 | 0;
    HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
    HEAP32[$10 >> 2] = HEAP32[$10 >> 2] + 1;
    $5 = HEAP32[$0 + 4 >> 2];
    HEAP32[$4 + 4 >> 2] = $5;
    HEAP32[$5 >> 2] = $4;
    $4 = HEAP32[$6 >> 2];
    HEAP32[$4 + 4 >> 2] = $0;
    HEAP32[$6 >> 2] = $0;
    HEAP32[$0 >> 2] = $4;
    HEAP32[$0 + 4 >> 2] = $6;
    $4 = HEAP32[$0 + 24 >> 2] + -1 | 0;
    HEAP32[$0 + 24 >> 2] = $4;
    label$3 : {
     if ($4) {
      break label$3
     }
     $5 = HEAP32[$0 >> 2];
     $4 = HEAP32[$0 >> 2];
     HEAP32[$6 >> 2] = $4;
     $5 = $5 & 1 ? $5 + -1 | 0 : $3;
     HEAP32[$5 >> 2] = HEAP32[$5 >> 2] & -17;
     HEAP32[$8 >> 2] = HEAP32[$8 >> 2] + -1;
     $5 = $4 & 1 ? $4 + -1 | 0 : $3;
     HEAP32[$5 >> 2] = HEAP32[$5 >> 2] & -33;
     HEAP32[$9 >> 2] = HEAP32[$9 >> 2] + -1;
     HEAP32[$4 + 4 >> 2] = $6;
     $4 = ((Math_imul(HEAP32[$3 >> 2] >>> 31 | 0, 516) + $7 | 0) + $11 | 0) + 468 | 0;
     HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + -1;
     HEAP32[$0 >> 2] = 256;
     $4 = $0 + 4 | 0;
     HEAP32[$4 >> 2] = 512;
     HEAP32[$10 >> 2] = HEAP32[$10 >> 2] + -1;
     label$4 : {
      if (HEAP32[$3 >> 2] & 65536) {
       break label$4
      }
      if (HEAP32[$0 >> 2] & 1) {
       break label$4
      }
      $3 = HEAP32[$2 >> 2];
      HEAP32[$3 + 4 >> 2] = $0;
      HEAP32[$0 >> 2] = $3;
      HEAP32[$2 >> 2] = $0;
      HEAP32[$4 >> 2] = $2;
      break label$3;
     }
     HEAP8[82796] = 1;
     FUNCTION_TABLE[HEAP32[(HEAPU8[$0 + 40 | 0] << 2) + 26476 >> 2]]($3);
     HEAP8[82796] = 0;
    }
    if (HEAP32[$1 >> 2] != ($1 | 0)) {
     continue
    }
    break;
   };
  }
 }
 
 function putback_inactive_pages($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  $3 = HEAP32[$1 >> 2];
  HEAP32[$6 + 12 >> 2] = $6 + 8;
  HEAP32[$6 + 8 >> 2] = $6 + 8;
  if (($1 | 0) != ($3 | 0)) {
   $12 = $0 + -1112 | 0;
   $14 = $1 + 4 | 0;
   while (1) {
    $3 = HEAP32[$14 >> 2];
    $4 = HEAP32[$3 >> 2];
    $2 = HEAP32[$3 + 4 >> 2];
    HEAP32[$4 + 4 >> 2] = $2;
    HEAP32[$2 >> 2] = $4;
    HEAP32[$3 >> 2] = 256;
    HEAP32[$3 + 4 >> 2] = 512;
    label$3 : {
     label$4 : {
      label$5 : {
       $4 = $3 + -4 | 0;
       $2 = page_mapping($4);
       if (HEAP32[$2 + 56 >> 2] & 8 ? $2 : 0) {
        break label$5
       }
       $2 = HEAP32[$3 >> 2];
       if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $4) >> 2] & 2097152) {
        break label$5
       }
       $2 = HEAP32[$3 >> 2];
       $2 = $2 & 1 ? $2 + -1 | 0 : $4;
       HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 16;
       $2 = 4;
       $5 = HEAP32[$3 >> 2];
       if (!(HEAP32[($5 & 1 ? $5 + -1 | 0 : $4) >> 2] & 1048576)) {
        $2 = HEAP32[$3 >> 2];
        $5 = HEAP32[($2 & 1 ? $2 + -1 | 0 : $4) >> 2] >>> 5 & 1;
        $2 = HEAP32[$3 >> 2];
        $2 = ($5 | HEAP32[($2 & 1 ? $2 + -1 | 0 : $4) >> 2] >>> 18 & 2) ^ 2;
       }
       $5 = $2 << 2;
       $7 = ($5 + $12 | 0) + 1184 | 0;
       HEAP32[$7 >> 2] = HEAP32[$7 >> 2] + 1;
       $10 = $5 + 82672 | 0;
       HEAP32[$10 >> 2] = HEAP32[$10 >> 2] + 1;
       $13 = $2 + 1 << 2;
       $5 = ($13 + (Math_imul(HEAP32[$4 >> 2] >>> 31 | 0, 516) + $12 | 0) | 0) + 468 | 0;
       HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
       $5 = ($2 << 3) + $0 | 0;
       $8 = HEAP32[$5 >> 2];
       HEAP32[$8 + 4 >> 2] = $3;
       $11 = $13 + 82624 | 0;
       HEAP32[$11 >> 2] = HEAP32[$11 >> 2] + 1;
       HEAP32[$3 >> 2] = $8;
       $8 = $3 + 4 | 0;
       HEAP32[$8 >> 2] = $5;
       HEAP32[$5 >> 2] = $3;
       if (($2 | 2) == 3) {
        $2 = (((($2 | 1) == 3) << 2) + $0 | 0) + 40 | 0;
        HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + 1;
       }
       $2 = HEAP32[$3 + 24 >> 2] + -1 | 0;
       HEAP32[$3 + 24 >> 2] = $2;
       if ($2) {
        break label$3
       }
       $9 = HEAP32[$3 >> 2];
       $2 = HEAP32[$3 >> 2];
       HEAP32[$5 >> 2] = $2;
       $9 = $9 & 1 ? $9 + -1 | 0 : $4;
       HEAP32[$9 >> 2] = HEAP32[$9 >> 2] & -17;
       HEAP32[$7 >> 2] = HEAP32[$7 >> 2] + -1;
       $7 = $2 & 1 ? $2 + -1 | 0 : $4;
       HEAP32[$7 >> 2] = HEAP32[$7 >> 2] & -33;
       HEAP32[$10 >> 2] = HEAP32[$10 >> 2] + -1;
       HEAP32[$2 + 4 >> 2] = $5;
       $2 = ((Math_imul(HEAP32[$4 >> 2] >>> 31 | 0, 516) + $12 | 0) + $13 | 0) + 468 | 0;
       HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + -1;
       HEAP32[$3 >> 2] = 256;
       HEAP32[$8 >> 2] = 512;
       HEAP32[$11 >> 2] = HEAP32[$11 >> 2] + -1;
       if (HEAP32[$4 >> 2] & 65536) {
        break label$4
       }
       if (HEAP32[$3 >> 2] & 1) {
        break label$4
       }
       $4 = HEAP32[$6 + 8 >> 2];
       HEAP32[$4 + 4 >> 2] = $3;
       HEAP32[$3 >> 2] = $4;
       HEAP32[$6 + 8 >> 2] = $3;
       HEAP32[$8 >> 2] = $6 + 8;
       break label$3;
      }
      HEAP8[82796] = 1;
      lru_cache_add($4);
      $3 = HEAP32[$3 >> 2];
      $3 = $3 & 1 ? $3 + -1 | 0 : $4;
      $4 = HEAP32[$3 + 28 >> 2] + -1 | 0;
      HEAP32[$3 + 28 >> 2] = $4;
      if (!$4) {
       __put_page($3)
      }
      HEAP8[82796] = 0;
      break label$3;
     }
     HEAP8[82796] = 1;
     FUNCTION_TABLE[HEAP32[(HEAPU8[$3 + 40 | 0] << 2) + 26476 >> 2]]($4);
     HEAP8[82796] = 0;
    }
    if (HEAP32[$1 >> 2] != ($1 | 0)) {
     continue
    }
    break;
   };
  }
  $0 = HEAP32[$6 + 8 >> 2];
  if (($0 | 0) != ($6 + 8 | 0)) {
   HEAP32[$1 >> 2] = $0;
   $3 = HEAP32[$6 + 12 >> 2];
   HEAP32[$0 + 4 >> 2] = $1;
   HEAP32[$3 >> 2] = $1;
   HEAP32[$1 + 4 >> 2] = $3;
  }
  global$0 = $6 + 16 | 0;
 }
 
 function __put_page($0) {
  label$1 : {
   if (HEAP32[$0 >> 2] & 65536) {
    break label$1
   }
   if (HEAP32[$0 + 4 >> 2] & 1) {
    break label$1
   }
   __page_cache_release($0);
   free_unref_page($0);
   return;
  }
  __page_cache_release($0);
  FUNCTION_TABLE[HEAP32[(HEAPU8[$0 + 44 | 0] << 2) + 26476 >> 2]]($0);
 }
 
 function __page_cache_release($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $1 = HEAP32[$0 + 4 >> 2];
  if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 16) {
   $7 = HEAP32[20700];
   $5 = HEAP32[$0 >> 2];
   HEAP32[20700] = 0;
   $3 = 4;
   $4 = $0 + 4 | 0;
   $1 = HEAP32[$4 >> 2];
   $1 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -17;
   $1 = HEAP32[$4 >> 2];
   $2 = $1 & 1 ? $1 + -1 | 0 : $0;
   $5 = HEAP32[Math_imul($5 >>> 31 | 0, 516) + 141296 >> 2] + 1112 | 0;
   $4 = HEAP32[$4 >> 2];
   label$2 : {
    if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $0) >> 2] & 1048576)) {
     $3 = (HEAP32[$2 >> 2] >>> 18 ^ -1) & 2;
     $2 = $0 + 4 | 0;
     $1 = HEAP32[$2 >> 2];
     if (!(HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 32)) {
      break label$2
     }
     $1 = HEAP32[$2 >> 2];
     $2 = $1 & 1 ? $1 + -1 | 0 : $0;
     HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -33;
     $3 = $3 | 1;
     break label$2;
    }
    HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -1048577;
   }
   $2 = $0 + 8 | 0;
   $4 = HEAP32[$2 >> 2];
   HEAP32[$4 >> 2] = $1;
   $5 = $5 + -1112 | 0;
   $3 = $3 << 2;
   $6 = ($5 + $3 | 0) + 1184 | 0;
   HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + -1;
   $6 = $3 + 82672 | 0;
   HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + -1;
   HEAP32[$1 + 4 >> 2] = $4;
   $1 = ($3 + ($5 + Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) | 0) | 0) + 472 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
   HEAP32[$0 + 4 >> 2] = 256;
   HEAP32[20700] = $7;
   HEAP32[$2 >> 2] = 512;
   $1 = $3 + 82628 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
  }
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -129;
 }
 
 function pagevec_move_tail() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  if (HEAPU8[16572]) {
   $7 = 16576;
   while (1) {
    $2 = HEAP32[$7 >> 2];
    if (($4 | 0) != 141272) {
     if ($4) {
      HEAP32[20700] = $9
     }
     $9 = HEAP32[20700];
     HEAP32[20700] = 0;
    }
    $4 = HEAP32[$2 + 4 >> 2];
    label$5 : {
     if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $2) >> 2] & 16)) {
      break label$5
     }
     $4 = $2 + 4 | 0;
     $0 = HEAP32[$4 >> 2];
     if (HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] & 1048576) {
      break label$5
     }
     $6 = 4;
     $3 = 4;
     $0 = HEAP32[$2 + 4 >> 2];
     if (!(HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] & 1048576)) {
      $0 = HEAP32[$4 >> 2];
      $3 = HEAP32[$4 >> 2];
      $3 = (HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] >>> 5 & 1 | HEAP32[($3 & 1 ? $3 + -1 | 0 : $2) >> 2] >>> 18 & 2) ^ 2;
     }
     $8 = $2 + 8 | 0;
     $5 = HEAP32[$8 >> 2];
     HEAP32[$5 >> 2] = $0;
     $3 = $3 << 2;
     $1 = $3 + 142456 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
     HEAP32[$2 + 4 >> 2] = 256;
     $1 = $3 + 82628 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
     $1 = $3 + 82672 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
     $1 = $3;
     $3 = Math_imul(HEAP32[$2 >> 2] >>> 31 | 0, 516);
     $1 = ($1 + $3 | 0) + 141744 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
     HEAP32[$0 + 4 >> 2] = $5;
     $0 = HEAP32[$2 + 4 >> 2];
     $0 = $0 & 1 ? $0 + -1 | 0 : $2;
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -33;
     HEAP32[$8 >> 2] = 512;
     $0 = HEAP32[$2 + 4 >> 2];
     if (!(HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] & 1048576)) {
      $0 = HEAP32[$4 >> 2];
      $1 = HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] >>> 5 & 1;
      $0 = HEAP32[$4 >> 2];
      $6 = ($1 | HEAP32[($0 & 1 ? $0 + -1 | 0 : $2) >> 2] >>> 18 & 2) ^ 2;
     }
     $0 = $6 << 2;
     $5 = $0 + 142456 | 0;
     HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
     $5 = $6 << 3;
     $1 = $5 + 142388 | 0;
     $6 = HEAP32[$1 >> 2];
     HEAP32[$1 >> 2] = $4;
     $1 = $0 + 82672 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
     $1 = $0 + 82628 | 0;
     HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
     $0 = ($0 + $3 | 0) + 141744 | 0;
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
     HEAP32[$2 + 4 >> 2] = $5 + 142384;
     HEAP32[$8 >> 2] = $6;
     HEAP32[$6 >> 2] = $4;
    }
    $7 = $7 + 4 | 0;
    $4 = 141272;
    $10 = $10 + 1 | 0;
    if ($10 >>> 0 < HEAPU8[16572]) {
     continue
    }
    break;
   };
   HEAP32[20700] = $9;
   $3 = HEAPU8[16572];
  } else {
   $3 = 0
  }
  release_pages(16576, $3);
  HEAP8[16572] = 0;
 }
 
 function release_pages($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  HEAP32[$7 + 12 >> 2] = $7 + 8;
  HEAP32[$7 + 8 >> 2] = $7 + 8;
  label$1 : {
   if (($1 | 0) < 1) {
    break label$1
   }
   while (1) {
    $4 = HEAP32[$0 >> 2];
    label$3 : {
     if ($2) {
      $8 = $8 + 1 | 0;
      if (($8 | 0) != 32) {
       break label$3
      }
      $2 = 0;
      HEAP32[20700] = $9;
      $8 = 32;
      break label$3;
     }
     $2 = 0;
    }
    $5 = HEAP32[$4 + 4 >> 2];
    $4 = $5 & 1 ? $5 + -1 | 0 : $4;
    $5 = HEAP32[$4 + 28 >> 2] + -1 | 0;
    HEAP32[$4 + 28 >> 2] = $5;
    label$5 : {
     if ($5) {
      break label$5
     }
     label$6 : {
      label$7 : {
       label$8 : {
        label$9 : {
         if (HEAP32[$4 >> 2] & 65536) {
          break label$9
         }
         if (HEAP32[$4 + 4 >> 2] & 1) {
          break label$9
         }
         $5 = $4 + 4 | 0;
         $3 = HEAP32[$5 >> 2];
         if (!(HEAP32[($3 & 1 ? $3 + -1 | 0 : $4) >> 2] & 16)) {
          break label$6
         }
         if (($2 | 0) != 141272) {
          if ($2) {
           HEAP32[20700] = $9
          }
          $9 = HEAP32[20700];
          HEAP32[20700] = 0;
          $8 = 0;
         }
         $2 = HEAP32[$5 >> 2];
         $2 = $2 & 1 ? $2 + -1 | 0 : $4;
         HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -17;
         $2 = HEAP32[$5 >> 2];
         $3 = $2 & 1 ? $2 + -1 | 0 : $4;
         $6 = HEAP32[$5 >> 2];
         if (HEAP32[($6 & 1 ? $6 + -1 | 0 : $4) >> 2] & 1048576) {
          break label$8
         }
         $3 = (HEAP32[$3 >> 2] >>> 18 ^ -1) & 2;
         $2 = HEAP32[$5 >> 2];
         $6 = $3;
         if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $4) >> 2] & 32)) {
          break label$7
         }
         $2 = HEAP32[$5 >> 2];
         $6 = $2 & 1 ? $2 + -1 | 0 : $4;
         HEAP32[$6 >> 2] = HEAP32[$6 >> 2] & -33;
         $6 = $3 | 1;
         break label$7;
        }
        if ($2) {
         HEAP32[20700] = $9
        }
        __page_cache_release($4);
        FUNCTION_TABLE[HEAP32[(HEAPU8[$4 + 44 | 0] << 2) + 26476 >> 2]]($4);
        $2 = 0;
        break label$5;
       }
       HEAP32[$3 >> 2] = HEAP32[$3 >> 2] & -1048577;
       $6 = 4;
      }
      $3 = $6;
      $6 = $4 + 8 | 0;
      $10 = HEAP32[$6 >> 2];
      HEAP32[$10 >> 2] = $2;
      HEAP32[$2 + 4 >> 2] = $10;
      $2 = $3 << 2;
      $3 = $2 + 142456 | 0;
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + -1;
      HEAP32[$4 + 4 >> 2] = 256;
      HEAP32[$6 >> 2] = 512;
      $3 = $2 + 82628 | 0;
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + -1;
      $3 = $2 + 82672 | 0;
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + -1;
      $2 = ($2 + Math_imul(HEAP32[$4 >> 2] >>> 31 | 0, 516) | 0) + 141744 | 0;
      HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + -1;
      $2 = 141272;
     }
     $3 = HEAP32[$5 >> 2];
     $3 = $3 & 1 ? $3 + -1 | 0 : $4;
     HEAP32[$3 >> 2] = HEAP32[$3 >> 2] & -33;
     $3 = HEAP32[$7 + 8 >> 2];
     HEAP32[$3 + 4 >> 2] = $5;
     HEAP32[$5 >> 2] = $3;
     HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -129;
     HEAP32[$4 + 8 >> 2] = $7 + 8;
     HEAP32[$7 + 8 >> 2] = $5;
    }
    $0 = $0 + 4 | 0;
    $1 = $1 + -1 | 0;
    if ($1) {
     continue
    }
    break;
   };
   if (!$2) {
    break label$1
   }
   HEAP32[20700] = $9;
  }
  free_unref_page_list($7 + 8 | 0);
  global$0 = $7 + 16 | 0;
 }
 
 function __pagevec_lru_add() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  if (HEAPU8[16636]) {
   $7 = 16640;
   while (1) {
    $3 = HEAP32[$7 >> 2];
    if (($2 | 0) != 141272) {
     if ($2) {
      HEAP32[20700] = $8
     }
     $8 = HEAP32[20700];
     HEAP32[20700] = 0;
    }
    $2 = HEAP32[$3 + 4 >> 2];
    $2 = $2 & 1 ? $2 + -1 | 0 : $3;
    HEAP32[$6 + 12 >> 2] = HEAP32[$2 >> 2];
    if (HEAP32[$6 + 12 >> 2] & 1048576) {
     HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -1048577
    }
    $2 = $3 + 4 | 0;
    $1 = HEAP32[$2 >> 2];
    $1 = $1 & 1 ? $1 + -1 | 0 : $3;
    HEAP32[$1 >> 2] = HEAP32[$1 >> 2] | 16;
    $1 = page_evictable($3);
    $0 = HEAP32[$2 >> 2];
    $0 = $0 & 1 ? $0 + -1 | 0 : $3;
    label$6 : {
     if ($1) {
      $1 = 4;
      if (!(HEAP32[$0 >> 2] & 1048576)) {
       $1 = HEAP32[$2 >> 2];
       $0 = HEAP32[($1 & 1 ? $1 + -1 | 0 : $3) >> 2] >>> 5 & 1;
       $1 = HEAP32[$2 >> 2];
       $1 = ($0 | HEAP32[($1 & 1 ? $1 + -1 | 0 : $3) >> 2] >>> 18 & 2) ^ 2;
      }
      $0 = HEAP32[$2 >> 2];
      $0 = HEAP32[($0 & 1 ? $0 + -1 | 0 : $3) >> 2];
      $4 = HEAP32[$2 >> 2];
      $4 = ((HEAP32[($4 & 1 ? $4 + -1 | 0 : $3) >> 2] >>> 19 ^ -1) & 1) << 2;
      $5 = $4 + 142432 | 0;
      HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
      if (!($0 & 32)) {
       break label$6
      }
      $0 = $4 + 142424 | 0;
      HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
      break label$6;
     }
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -33;
     $1 = 4;
     $0 = HEAP32[$3 + 4 >> 2];
     $0 = $0 & 1 ? $0 + -1 | 0 : $3;
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 1048576;
    }
    $0 = $1 << 2;
    $4 = $0 + 142456 | 0;
    HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + 1;
    $1 = ($1 << 3) + 142384 | 0;
    $4 = HEAP32[$1 >> 2];
    HEAP32[$4 + 4 >> 2] = $2;
    $5 = ($0 + Math_imul(HEAP32[$3 >> 2] >>> 31 | 0, 516) | 0) + 141744 | 0;
    HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + 1;
    HEAP32[$3 + 4 >> 2] = $4;
    $4 = $0 + 82672 | 0;
    HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + 1;
    $0 = $0 + 82628 | 0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
    HEAP32[$3 + 8 >> 2] = $1;
    HEAP32[$1 >> 2] = $2;
    $7 = $7 + 4 | 0;
    $2 = 141272;
    $9 = $9 + 1 | 0;
    if ($9 >>> 0 < HEAPU8[16636]) {
     continue
    }
    break;
   };
   HEAP32[20700] = $8;
   $0 = HEAPU8[16636];
  } else {
   $0 = 0
  }
  release_pages(16640, $0);
  HEAP8[16636] = 0;
  global$0 = $6 + 16 | 0;
 }
 
 function lru_cache_add($0) {
  var $1 = 0, $2 = 0;
  $1 = HEAPU8[16636];
  $2 = $1 + 1 | 0;
  HEAP8[16636] = $2;
  HEAP32[($1 << 2) + 16640 >> 2] = $0;
  $1 = HEAP32[$0 + 4 >> 2];
  $1 = $1 & 1 ? $1 + -1 | 0 : $0;
  HEAP32[$1 + 28 >> 2] = HEAP32[$1 + 28 >> 2] + 1;
  label$1 : {
   label$2 : {
    if (($2 & 255) == 15) {
     break label$2
    }
    if (HEAP32[$0 >> 2] & 65536) {
     break label$2
    }
    if (!(HEAP32[$0 + 4 >> 2] & 1)) {
     break label$1
    }
   }
   __pagevec_lru_add();
  }
 }
 
 function lru_add_drain_cpu() {
  var $0 = 0, $1 = 0, $2 = 0;
  if (HEAPU8[16636]) {
   __pagevec_lru_add()
  }
  if (HEAPU8[16572]) {
   $0 = HEAP32[20700];
   HEAP32[20700] = 0;
   pagevec_move_tail();
   HEAP32[20700] = $0;
  }
  if (HEAPU8[16700]) {
   $2 = HEAP32[20700];
   HEAP32[20700] = 0;
   lru_deactivate_file_fn(HEAP32[4176]);
   if (HEAPU8[16700] >= 2) {
    $0 = 16708;
    $1 = 1;
    while (1) {
     lru_deactivate_file_fn(HEAP32[$0 >> 2]);
     $0 = $0 + 4 | 0;
     $1 = $1 + 1 | 0;
     if ($1 >>> 0 < HEAPU8[16700]) {
      continue
     }
     break;
    };
   }
   HEAP32[20700] = $2;
   release_pages(16704, HEAPU8[16700]);
   HEAP8[16700] = 0;
  }
  if (HEAPU8[16764]) {
   $2 = HEAP32[20700];
   HEAP32[20700] = 0;
   lru_lazyfree_fn(HEAP32[4192]);
   if (HEAPU8[16764] >= 2) {
    $0 = 16772;
    $1 = 1;
    while (1) {
     lru_lazyfree_fn(HEAP32[$0 >> 2]);
     $0 = $0 + 4 | 0;
     $1 = $1 + 1 | 0;
     if ($1 >>> 0 < HEAPU8[16764]) {
      continue
     }
     break;
    };
   }
   HEAP32[20700] = $2;
   release_pages(16768, HEAPU8[16764]);
   HEAP8[16764] = 0;
  }
 }
 
 function lru_deactivate_file_fn($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  label$1 : {
   $2 = HEAP32[$0 + 4 >> 2];
   if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 16)) {
    break label$1
   }
   $2 = HEAP32[$0 + 4 >> 2];
   if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 1048576) {
    break label$1
   }
   if (page_mapped($0)) {
    break label$1
   }
   $2 = $0 + 4 | 0;
   $3 = HEAP32[$2 >> 2];
   $1 = HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2];
   $3 = HEAP32[$2 >> 2];
   $7 = HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2];
   $3 = HEAP32[$2 >> 2];
   $4 = HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2];
   $8 = $0 + 8 | 0;
   $9 = HEAP32[$8 >> 2];
   HEAP32[$9 >> 2] = $3;
   $4 = ($4 >>> 18 ^ -1) & 2;
   $1 = ($4 | $1 >>> 5 & 1) << 2;
   $5 = $1 + 142456 | 0;
   HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + -1;
   $5 = $1 + 82672 | 0;
   HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + -1;
   $5 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141740 | 0;
   $6 = ($5 + $1 | 0) + 4 | 0;
   HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + -1;
   HEAP32[$2 >> 2] = 256;
   $1 = $1 + 82628 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
   $1 = HEAP32[$2 >> 2];
   $1 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -33;
   $1 = HEAP32[$2 >> 2];
   $1 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -3;
   $1 = $4 << 2;
   $6 = $1 + 142456 | 0;
   HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + 1;
   $6 = $1 + 82672 | 0;
   HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + 1;
   HEAP32[$3 + 4 >> 2] = $9;
   $3 = $1 | 4;
   $1 = $3 + $5 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + 1;
   HEAP32[$8 >> 2] = 512;
   $3 = $3 + 82624 | 0;
   HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + 1;
   $3 = ($4 << 3) + 142384 | 0;
   $1 = HEAP32[$3 >> 2];
   HEAP32[$1 + 4 >> 2] = $2;
   HEAP32[$2 >> 2] = $1;
   HEAP32[$3 >> 2] = $2;
   HEAP32[$8 >> 2] = $3;
   $8 = ($7 >>> 19 ^ -1) & 1;
   label$2 : {
    label$3 : {
     $7 = HEAP32[$2 >> 2];
     if (HEAP32[($7 & 1 ? $7 + -1 | 0 : $0) >> 2] & 32768) {
      break label$3
     }
     $7 = HEAP32[$2 >> 2];
     if (HEAP32[($7 & 1 ? $7 + -1 | 0 : $0) >> 2] & 8) {
      break label$3
     }
     HEAP32[$1 + 4 >> 2] = $3;
     HEAP32[$3 >> 2] = $1;
     $1 = ($4 << 3) + 142384 | 0;
     $4 = HEAP32[$1 + 4 >> 2];
     HEAP32[$1 + 4 >> 2] = $2;
     HEAP32[$0 + 4 >> 2] = $3;
     HEAP32[$4 >> 2] = $2;
     HEAP32[$0 + 8 >> 2] = $4;
     break label$2;
    }
    $2 = HEAP32[$2 >> 2];
    $0 = $2 & 1 ? $2 + -1 | 0 : $0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 262144;
   }
   $0 = ($8 << 2) + 142432 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
  }
 }
 
 function lru_lazyfree_fn($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  label$1 : {
   $2 = HEAP32[$0 + 4 >> 2];
   if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 16)) {
    break label$1
   }
   $2 = $0 + 4 | 0;
   $3 = HEAP32[$2 >> 2];
   if (!(HEAP8[($3 & 1 ? $3 + -1 | 0 : $0) + 12 | 0] & 1)) {
    break label$1
   }
   $2 = HEAP32[$2 >> 2];
   if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 524288)) {
    break label$1
   }
   $2 = $0 + 4 | 0;
   $3 = HEAP32[$2 >> 2];
   if (HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2] & 1048576) {
    break label$1
   }
   $3 = HEAP32[$2 >> 2];
   $1 = HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2];
   $5 = $0 + 8 | 0;
   $6 = HEAP32[$5 >> 2];
   HEAP32[$6 >> 2] = $3;
   $1 = $1 >>> 3 & 4;
   $4 = $1 + 142456 | 0;
   HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + -1;
   $4 = $1 + 82672 | 0;
   HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + -1;
   $4 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141272 | 0;
   $7 = ($4 + $1 | 0) + 472 | 0;
   HEAP32[$7 >> 2] = HEAP32[$7 >> 2] + -1;
   HEAP32[$2 >> 2] = 256;
   $1 = $1 + 82628 | 0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] + -1;
   $1 = HEAP32[$2 >> 2];
   $1 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -33;
   $1 = HEAP32[$2 >> 2];
   $1 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & -3;
   $1 = HEAP32[$2 >> 2];
   $0 = $1 & 1 ? $1 + -1 | 0 : $0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -524289;
   HEAP32[20670] = HEAP32[20670] + 1;
   HEAP32[35616] = HEAP32[35616] + 1;
   HEAP32[$3 + 4 >> 2] = $6;
   $0 = $4 + 480 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
   HEAP32[$5 >> 2] = 512;
   HEAP32[20659] = HEAP32[20659] + 1;
   $0 = HEAP32[35600];
   HEAP32[$0 + 4 >> 2] = $2;
   HEAP32[$2 >> 2] = $0;
   HEAP32[35600] = $2;
   HEAP32[$5 >> 2] = 142400;
   HEAP32[35609] = HEAP32[35609] + 1;
  }
 }
 
 function node_dirty_ok($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  if (HEAP32[$0 + 48 >> 2]) {
   $1 = HEAP32[$0 + 468 >> 2]
  } else {
   $1 = 0
  }
  $1 = HEAP32[$0 + 564 >> 2] ? HEAP32[$0 + 984 >> 2] + $1 | 0 : $1;
  $0 = HEAP32[$0 + 1108 >> 2];
  $0 = HEAP32[20671] + (($1 + HEAP32[20670] | 0) - ($1 >>> 0 < $0 >>> 0 ? $1 : $0) | 0) | 0;
  $1 = HEAP32[2];
  $2 = HEAP32[20701];
  label$4 : {
   if ($2) {
    $3 = Math_imul($0, $2 + 65535 >>> 16 | 0);
    $0 = HEAP32[20656];
    $2 = HEAP32[20704];
    $0 = ($3 >>> 0) / (HEAP32[20671] + ((($0 + HEAP32[20670] | 0) + 1 | 0) - ($0 >>> 0 < $2 >>> 0 ? $0 : $2) | 0) >>> 0) | 0;
    break label$4;
   }
   $0 = (Math_imul($0, HEAP32[4211]) >>> 0) / 100 | 0;
  }
  if (!(HEAP32[$1 + 24 >> 2] > 99 ? !(HEAPU8[$1 + 14 | 0] & 16) : 0)) {
   $0 = ($0 >>> 2 | 0) + $0 | 0
  }
  return HEAP32[20686] + (HEAP32[20685] + HEAP32[20692] | 0) >>> 0 <= $0 >>> 0;
 }
 
 function clear_page_dirty_for_io($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $1 = page_mapping($0);
  $2 = HEAP32[$0 + 4 >> 2];
  if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 1) {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       if (!$1) {
        break label$5
       }
       $1 = HEAP32[$1 >> 2];
       label$6 : {
        if ($1) {
         $2 = HEAP32[HEAP32[$1 + 20 >> 2] + 108 >> 2];
         break label$6;
        }
        $2 = 16184;
       }
       if (HEAP8[$2 + 32 | 0] & 1) {
        break label$5
       }
       if (page_mkclean($0)) {
        set_page_dirty($0)
       }
       if (!$1) {
        break label$4
       }
       $2 = HEAP32[HEAP32[$1 + 20 >> 2] + 108 >> 2];
       break label$3;
      }
      $1 = HEAP32[$0 + 4 >> 2];
      $2 = $1 & 1 ? $1 + -1 | 0 : $0;
      $0 = HEAP32[$2 >> 2];
      HEAP32[$3 + 12 >> 2] = $0;
      $1 = 0;
      if (!(HEAP32[$3 + 12 >> 2] & 8)) {
       break label$2
      }
      HEAP32[$2 >> 2] = $0 & -9;
      $1 = $0 >>> 3 & 1;
      break label$2;
     }
     $2 = 16184;
    }
    $1 = HEAP32[$0 + 4 >> 2];
    $4 = $1 & 1 ? $1 + -1 | 0 : $0;
    $5 = HEAP32[$4 >> 2];
    HEAP32[$3 + 12 >> 2] = $5;
    $1 = 0;
    if (!(HEAP32[$3 + 12 >> 2] & 8)) {
     break label$2
    }
    HEAP32[$4 >> 2] = $5 & -9;
    if (!($5 & 8)) {
     break label$2
    }
    HEAP32[35631] = HEAP32[35631] + -1;
    HEAP32[20685] = HEAP32[20685] + -1;
    HEAP32[20662] = HEAP32[20662] + -1;
    $0 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141764 | 0;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
    $1 = $2 + 104 | 0;
    $2 = $1;
    $4 = $1;
    $0 = HEAP32[$1 + 4 >> 2] + -1 | 0;
    $1 = HEAP32[$1 >> 2] + -1 | 0;
    if ($1 >>> 0 < 4294967295) {
     $0 = $0 + 1 | 0
    }
    HEAP32[$4 >> 2] = $1;
    HEAP32[$2 + 4 >> 2] = $0;
    $1 = 1;
   }
   global$0 = $3 + 16 | 0;
   return $1;
  }
  HEAP32[$3 + 8 >> 2] = 26442;
  HEAP32[$3 + 4 >> 2] = 2639;
  HEAP32[$3 >> 2] = 26425;
  printk(26394, $3);
  abort();
 }
 
 function set_page_dirty($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $1 = page_mapping($0);
  $2 = HEAP32[$0 + 4 >> 2];
  $0 = $2 & 1 ? $2 + -1 | 0 : $0;
  label$1 : {
   if ($1) {
    $1 = HEAP32[HEAP32[$1 + 52 >> 2] + 12 >> 2];
    $2 = HEAP32[$0 + 4 >> 2];
    if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $0) >> 2] & 262144) {
     $2 = HEAP32[$0 + 4 >> 2];
     $2 = $2 & 1 ? $2 + -1 | 0 : $0;
     HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -262145;
    }
    FUNCTION_TABLE[$1]($0) | 0;
    break label$1;
   }
   $1 = HEAP32[$0 + 4 >> 2];
   label$4 : {
    if (HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 8) {
     break label$4
    }
    $1 = HEAP32[$0 + 4 >> 2];
    $1 = $1 & 1 ? $1 + -1 | 0 : $0;
    $0 = HEAP32[$1 >> 2];
    HEAP32[$3 + 12 >> 2] = $0;
    if (HEAP32[$3 + 12 >> 2] & 8) {
     break label$4
    }
    HEAP32[$1 >> 2] = $0 | 8;
    if (!($0 & 8)) {
     break label$1
    }
   }
  }
  global$0 = $3 + 16 | 0;
 }
 
 function account_page_cleaned($0, $1, $2) {
  $1 = HEAP32[$1 >> 2];
  label$1 : {
   if ($1) {
    $1 = HEAP32[HEAP32[$1 + 20 >> 2] + 108 >> 2];
    break label$1;
   }
   $1 = 16184;
  }
  if (!(HEAP8[$1 + 32 | 0] & 1)) {
   HEAP32[35631] = HEAP32[35631] + -1;
   HEAP32[20685] = HEAP32[20685] + -1;
   HEAP32[20662] = HEAP32[20662] + -1;
   $0 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141764 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + -1;
   $0 = HEAP32[$2 + 52 >> 2] + -1 | 0;
   $1 = HEAP32[$2 + 48 >> 2] + -1 | 0;
   if ($1 >>> 0 < 4294967295) {
    $0 = $0 + 1 | 0
   }
   HEAP32[$2 + 48 >> 2] = $1;
   HEAP32[$2 + 52 >> 2] = $0;
  }
 }
 
 function free_compound_page($0) {
  $0 = $0 | 0;
  __free_pages_ok($0, HEAP32[$0 >> 2] & 65536 ? HEAPU8[$0 + 45 | 0] : 0);
 }
 
 function __free_pages_ok($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $4 = ($0 - HEAP32[20646] | 0) / 36 | 0;
  label$1 : {
   if (!$1) {
    break label$1
   }
   $11 = HEAP32[$0 >> 2] & 65536 ? 1 : HEAP32[$0 + 4 >> 2] & 1;
   $2 = 1 << $1;
   if (($2 | 0) < 2) {
    break label$1
   }
   $5 = $2 + -1 | 0;
   $2 = $0 + 36 | 0;
   while (1) {
    if ($11) {
     HEAP32[$2 + 4 >> 2] = 0;
     HEAP32[$2 + 12 >> 2] = 0;
    }
    label$5 : {
     label$6 : {
      $6 = $2 + 24 | 0;
      label$7 : {
       if (HEAP32[$6 >> 2] != -1) {
        $7 = HEAP32[$2 >> 2] & 3207729;
        $8 = HEAP32[$2 + 12 >> 2];
        break label$7;
       }
       $3 = HEAP32[$2 >> 2];
       $7 = $3 & 3207729;
       $8 = HEAP32[$2 + 12 >> 2];
       if (!($7 | ($8 | HEAP32[$2 + 28 >> 2]))) {
        break label$6
       }
      }
      bad_page($2, $7 ? 27829 : HEAP32[$2 + 28 >> 2] ? 27811 : $8 ? 27794 : HEAP32[$6 >> 2] == -1 ? 0 : 27777, $7 ? 3207729 : 0);
      $10 = $10 + 1 | 0;
      break label$5;
     }
     HEAP32[$2 >> 2] = $3 & -4194304;
    }
    $2 = $2 + 36 | 0;
    $5 = $5 + -1 | 0;
    if ($5) {
     continue
    }
    break;
   };
  }
  $3 = $0 + 12 | 0;
  $2 = HEAP32[$3 >> 2];
  if ($2 & 3) {
   HEAP32[$3 >> 2] = 0;
   $2 = 0;
  }
  label$10 : {
   label$11 : {
    if (HEAP32[$0 + 24 >> 2] == -1) {
     $5 = 0;
     $3 = HEAP32[$0 >> 2] & 3207729;
     if ($3 | (HEAP32[$0 + 28 >> 2] | $2)) {
      break label$11
     }
     break label$10;
    }
    $3 = HEAP32[$0 >> 2] & 3207729;
   }
   bad_page($0, $3 ? 27829 : HEAP32[$0 + 28 >> 2] ? 27811 : $2 ? 27794 : HEAP32[$0 + 24 >> 2] == -1 ? 0 : 27777, $3 ? 3207729 : 0);
   $5 = 1;
  }
  if (($5 | 0) == (0 - $10 | 0)) {
   $2 = HEAP32[$0 >> 2];
   HEAP32[$0 >> 2] = $2 & -4194304;
   $7 = $2 >>> 31 | 0;
   $6 = Math_imul($7, 516);
   $3 = $4 - (HEAP32[$6 + 141308 >> 2] & -1024) | 0;
   $10 = HEAP32[HEAP32[$6 + 141304 >> 2] + ($3 >>> 11 & 2097148) >> 2];
   $2 = $6 + 141740 | 0;
   $11 = $2;
   $9 = HEAP32[$2 >> 2];
   $2 = 1 << $1;
   HEAP32[$11 >> 2] = $9 + $2;
   HEAP32[20656] = $2 + HEAP32[20656];
   $11 = $10 >>> 29 - ($3 >>> 8 & 28) | 0;
   label$14 : {
    label$15 : {
     if ($1 >>> 0 <= 9) {
      $5 = ($6 + Math_imul($1, 36) | 0) + 141364 | 0;
      while (1) {
       $10 = 1 << $1 ^ $4;
       $9 = Math_imul($10 - $4 | 0, 36) + $0 | 0;
       if ((HEAP32[$9 + 24 >> 2] & -268435328) != -268435456) {
        break label$15
       }
       $8 = $9 + 20 | 0;
       if (HEAP32[$8 >> 2] != ($1 | 0)) {
        break label$15
       }
       if ((HEAP32[$9 >> 2] ^ HEAP32[$0 >> 2]) < 0) {
        break label$15
       }
       HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + -1;
       $6 = $9 + 8 | 0;
       $3 = HEAP32[$6 >> 2];
       $2 = HEAP32[$9 + 4 >> 2];
       HEAP32[$3 >> 2] = $2;
       HEAP32[$2 + 4 >> 2] = $3;
       HEAP32[$9 + 4 >> 2] = 256;
       HEAP32[$8 >> 2] = 0;
       HEAP32[$6 >> 2] = 512;
       $2 = $9 + 24 | 0;
       HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 128;
       $5 = $5 + 36 | 0;
       $2 = $4 & $10;
       $0 = Math_imul($2 - $4 | 0, 36) + $0 | 0;
       $4 = $2;
       $1 = $1 + 1 | 0;
       if ($1 >>> 0 < 10) {
        continue
       }
       break;
      };
      break label$14;
     }
     $2 = $4;
     break label$14;
    }
    $2 = $4;
   }
   $8 = $11 & 7;
   HEAP32[$0 + 20 >> 2] = $1;
   HEAP32[$0 + 24 >> 2] = HEAP32[$0 + 24 >> 2] & -129;
   label$18 : {
    label$19 : {
     if ($1 >>> 0 > 8) {
      break label$19
     }
     $6 = $2 & $10;
     $3 = Math_imul($6 - $2 | 0, 36) + $0 | 0;
     $4 = $1 + 1 | 0;
     $2 = $3 + Math_imul(($6 ^ 1 << $4) - $6 | 0, 36) | 0;
     if ((HEAP32[$2 + 24 >> 2] & -268435328) != -268435456) {
      break label$19
     }
     if (($4 | 0) != HEAP32[$2 + 20 >> 2]) {
      break label$19
     }
     if ((HEAP32[$2 >> 2] ^ HEAP32[$3 >> 2]) < 0) {
      break label$19
     }
     $3 = (Math_imul($7, 516) + Math_imul($1, 36) | 0) + ($8 << 3) | 0;
     $4 = $3 + 141336 | 0;
     $2 = HEAP32[$4 >> 2];
     $11 = $4;
     $4 = $0 + 4 | 0;
     HEAP32[$11 >> 2] = $4;
     $5 = $3 + 141332 | 0;
     break label$18;
    }
    $2 = ((Math_imul($7, 516) + Math_imul($1, 36) | 0) + ($8 << 3) | 0) + 141332 | 0;
    $5 = HEAP32[$2 >> 2];
    $4 = $0 + 4 | 0;
    HEAP32[$5 + 4 >> 2] = $4;
   }
   HEAP32[$0 + 4 >> 2] = $5;
   HEAP32[$2 >> 2] = $4;
   HEAP32[$0 + 8 >> 2] = $2;
   $0 = (Math_imul($7, 516) + Math_imul($1, 36) | 0) + 141364 | 0;
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
  }
 }
 
 function set_pageblock_migratetype($0) {
  var $1 = 0;
  $1 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516);
  $0 = (($0 - HEAP32[20646] | 0) / 36 | 0) - (HEAP32[$1 + 141308 >> 2] & -1024) | 0;
  $1 = HEAP32[$1 + 141304 >> 2] + ($0 >>> 11 & 2097148) | 0;
  $0 = 29 - ($0 >>> 8 & 28) | 0;
  HEAP32[$1 >> 2] = HEAP32[$1 >> 2] & (7 << $0 ^ -1) | (HEAP32[20703] ? 0 : 1) << $0;
 }
 
 function bad_page($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = global$0 - 48 | 0;
  global$0 = $3;
  HEAP32[$3 + 44 >> 2] = $2;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      $4 = HEAP32[20708];
      if (($4 | 0) == 60) {
       $4 = HEAP32[20709];
       if ((HEAP32[20745] - HEAP32[20707] | 0) > -1) {
        break label$4
       }
       HEAP32[20709] = $4 + 1;
       break label$1;
      }
      HEAP32[20708] = $4 + 1;
      if (!$4) {
       break label$3
      }
      break label$2;
     }
     if ($4) {
      HEAP32[$3 + 32 >> 2] = $4;
      printk(27866, $3 + 32 | 0);
      HEAP32[20709] = 0;
     }
     HEAP32[20708] = 1;
    }
    HEAP32[20707] = HEAP32[20745] + 15e3;
   }
   HEAP32[$3 + 16 >> 2] = HEAP32[2] + 700;
   HEAP32[$3 + 20 >> 2] = ($0 - HEAP32[20646] | 0) / 36;
   printk(27914, $3 + 16 | 0);
   __dump_page($0, $1);
   $1 = HEAP32[$0 >> 2] & $2;
   HEAP32[$3 + 44 >> 2] = $1;
   if ($1) {
    HEAP32[$3 >> 2] = $1;
    HEAP32[$3 + 4 >> 2] = $3 + 44;
    printk(27962, $3);
   }
   dump_stack();
  }
  HEAP32[$0 + 24 >> 2] = -1;
  add_taint(5, 1);
  global$0 = $3 + 48 | 0;
 }
 
 function reserve_bootmem_region($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $0 = $0 >>> 16 | 0;
  $3 = $1 + 65535 >>> 16 | 0;
  if ($0 >>> 0 < $3 >>> 0) {
   $1 = Math_imul($0, 36) + 4 | 0;
   while (1) {
    if ($0 >>> 0 < HEAPU32[20644]) {
     $2 = HEAP32[20646] + $1 | 0;
     HEAP32[$2 + 4 >> 2] = $2;
     $4 = $2 + -4 | 0;
     HEAP32[$4 >> 2] = HEAP32[$4 >> 2] | 4096;
     HEAP32[$2 >> 2] = $2;
    }
    $1 = $1 + 36 | 0;
    $0 = $0 + 1 | 0;
    if (($3 | 0) != ($0 | 0)) {
     continue
    }
    break;
   };
  }
 }
 
 function __free_pages_boot_core($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $2 = $0;
  $4 = 1 << $1;
  $3 = $4 + -1 | 0;
  if ($3) {
   while (1) {
    HEAP32[$2 + 28 >> 2] = 0;
    HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -4097;
    $2 = $2 + 36 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   }
  }
  HEAP32[$2 + 28 >> 2] = 0;
  HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & -4097;
  HEAP32[$0 + 28 >> 2] = 1;
  $2 = Math_imul(HEAP32[$0 >> 2] >>> 31 | 0, 516) + 141312 | 0;
  HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + $4;
  __free_pages($0, $1);
 }
 
 function __free_pages($0, $1) {
  var $2 = 0;
  $2 = HEAP32[$0 + 28 >> 2] + -1 | 0;
  HEAP32[$0 + 28 >> 2] = $2;
  if ($2) {
   return
  }
  if ($1) {
   __free_pages_ok($0, $1);
   return;
  }
  free_unref_page($0);
 }
 
 function move_freepages_block($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  if ($3) {
   HEAP32[$3 >> 2] = 0
  }
  $4 = HEAP32[20646];
  $5 = ($1 - $4 | 0) / 36 | 0;
  $6 = $5 | 1023;
  $5 = $5 & -1024;
  $8 = $4 + Math_imul($5, 36) | 0;
  $4 = HEAP32[$0 + 36 >> 2];
  if ($4 >>> 0 <= $5 >>> 0) {
   $1 = $4 + HEAP32[$0 + 44 >> 2] >>> 0 > $5 >>> 0 ? $8 : $1
  }
  label$3 : {
   if ($4 >>> 0 > $6 >>> 0) {
    break label$3
   }
   $5 = $8 + 36828 | 0;
   if ($1 >>> 0 > $5 >>> 0) {
    break label$3
   }
   if ($4 + HEAP32[$0 + 44 >> 2] >>> 0 <= $6 >>> 0) {
    break label$3
   }
   $8 = $2 << 3;
   while (1) {
    if ((HEAP32[$1 + 24 >> 2] & -268435328) != -268435456) {
     label$6 : {
      if (!$3) {
       break label$6
      }
      $2 = HEAP32[$1 + 4 >> 2];
      if (!(HEAP32[($2 & 1 ? $2 + -1 | 0 : $1) >> 2] & 16)) {
       if ((HEAP32[$1 + 12 >> 2] & 3) != 2) {
        break label$6
       }
      }
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + 1;
     }
     $1 = $1 + 36 | 0;
     if ($1 >>> 0 <= $5 >>> 0) {
      continue
     }
     break label$3;
    }
    $4 = HEAP32[$1 + 20 >> 2];
    $2 = $1 + 4 | 0;
    $6 = HEAP32[$2 >> 2];
    $9 = $1 + 8 | 0;
    $7 = HEAP32[$9 >> 2];
    HEAP32[$6 + 4 >> 2] = $7;
    HEAP32[$7 >> 2] = $6;
    $6 = ($8 + (Math_imul($4, 36) + $0 | 0) | 0) + 60 | 0;
    $7 = HEAP32[$6 >> 2];
    HEAP32[$7 + 4 >> 2] = $2;
    HEAP32[$6 >> 2] = $2;
    HEAP32[$2 >> 2] = $7;
    HEAP32[$9 >> 2] = $6;
    $2 = 1 << $4;
    $10 = $2 + $10 | 0;
    $1 = Math_imul($2, 36) + $1 | 0;
    if ($1 >>> 0 <= $5 >>> 0) {
     continue
    }
    break;
   };
  }
  return $10;
 }
 
 function free_pcppages_bulk($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0, $35 = 0, $36 = 0, $37 = 0, $38 = 0, $39 = 0, $40 = 0, $41 = 0, $42 = 0, $43 = 0, $44 = 0, $45 = 0, $46 = 0, $47 = 0, $48 = 0, $49 = 0, $50 = 0, $51 = 0, $52 = 0, $53 = 0, $54 = 0, $55 = 0;
  $8 = global$0 - 16 | 0;
  global$0 = $8;
  HEAP32[$8 + 12 >> 2] = $8 + 8;
  HEAP32[$8 + 8 >> 2] = $8 + 8;
  label$1 : {
   label$2 : {
    if ($1) {
     $13 = 1;
     $19 = 3;
     $33 = 12;
     $14 = -1;
     $34 = 16;
     $35 = 256;
     $36 = 512;
     $37 = -4;
     $20 = 3207729;
     $23 = 8;
     $38 = 4;
     $39 = 20;
     $40 = 27777;
     $41 = 27794;
     $42 = 27811;
     $43 = 27829;
     break label$2;
    }
    $3 = 0;
    break label$1;
   }
   $3 = 1;
  }
  label$4 : while (1) {
   label$5 : {
    label$6 : {
     label$7 : {
      label$8 : {
       label$9 : {
        label$10 : {
         if (!$3) {
          $15 = HEAP32[$8 + 8 >> 2];
          if (($15 | 0) == ($8 + 8 | 0)) {
           break label$7
          }
          $44 = 92;
          $45 = $0 + 92 | 0;
          $24 = $0 + 468 | 0;
          $11 = 1;
          $46 = -4;
          $9 = 36;
          $47 = 24;
          $25 = -268435328;
          $26 = -268435456;
          $21 = 20;
          $48 = -1;
          $22 = 8;
          $49 = 256;
          $50 = 512;
          $51 = 128;
          $52 = 10;
          $53 = -129;
          $27 = 3;
          $54 = 64;
          $28 = 4;
          $29 = 60;
          $6 = 1;
          break label$10;
         }
         if ($6) {
          break label$8
         }
         $16 = $13 + $16 | 0;
         $3 = $13 + $30 | 0;
         $30 = ($3 | 0) == ($19 | 0) ? 0 : $3;
         $3 = ($30 << $19) + $2 | 0;
         $7 = $3 + $33 | 0;
         if (HEAP32[$7 >> 2] == ($7 | 0)) {
          break label$9
         }
         $5 = $1 + $14 | 0;
         $4 = $13 - (($16 | 0) == ($19 | 0) ? $1 : $16) | 0;
         $17 = $3 + $34 | 0;
         while (1) {
          $3 = $4;
          $1 = $5;
          $4 = HEAP32[$17 >> 2];
          $5 = HEAP32[$4 + 4 >> 2];
          $10 = HEAP32[$4 >> 2];
          HEAP32[$5 >> 2] = $10;
          HEAP32[$10 + 4 >> 2] = $5;
          HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + $14;
          HEAP32[$4 >> 2] = $35;
          HEAP32[$4 + 4 >> 2] = $36;
          $10 = $4 + $37 | 0;
          label$15 : {
           label$16 : {
            label$17 : {
             if (($14 | 0) == HEAP32[$4 + 20 >> 2]) {
              $5 = HEAP32[$10 >> 2] & $20;
              $12 = HEAP32[$4 + $23 >> 2];
              if ($5 | ($12 | HEAP32[$4 + 24 >> 2])) {
               break label$17
              }
              $5 = HEAP32[$8 + 12 >> 2];
              HEAP32[$5 >> 2] = $4;
              HEAP32[$8 + 12 >> 2] = $4;
              HEAP32[$4 + $38 >> 2] = $5;
              HEAP32[$4 >> 2] = $8 + 8;
              $55 = $13 + $55 | 0;
              if (!$1) {
               break label$16
              }
              break label$15;
             }
             $5 = HEAP32[$10 >> 2] & $20;
             $12 = HEAP32[$4 + $23 >> 2];
            }
            bad_page($10, $5 ? $43 : HEAP32[$4 + 24 >> 2] ? $42 : $12 ? $41 : HEAP32[$4 + $39 >> 2] == ($14 | 0) ? 0 : $40, $5 ? $20 : 0);
            if ($1) {
             break label$15
            }
            $3 = 0;
            continue label$4;
           }
           $3 = 0;
           continue label$4;
          }
          $4 = $3 + $13 | 0;
          if (($4 | 0) == ($13 | 0)) {
           break label$5
          }
          $5 = $1 + $14 | 0;
          if (($7 | 0) != HEAP32[$7 >> 2]) {
           continue
          }
          break;
         };
         break label$5;
        }
        $3 = 1;
        continue;
       }
       $6 = 0;
       $3 = 1;
       continue;
      }
      $3 = $15;
      $10 = HEAP32[$3 + 12 >> 2];
      $15 = HEAP32[$3 >> 2];
      HEAP32[$24 >> 2] = HEAP32[$24 >> 2] + $11;
      HEAP32[20656] = HEAP32[20656] + $11;
      $7 = $3 + $46 | 0;
      $6 = ($7 - HEAP32[20646] | 0) / ($9 | 0) | 0;
      $5 = $45;
      $4 = 0;
      label$19 : {
       while (1) {
        label$20 : {
         $12 = $11 << $4 ^ $6;
         $3 = Math_imul($12 - $6 | 0, $9) + $7 | 0;
         if ((HEAP32[$3 + 24 >> 2] & $25) != ($26 | 0)) {
          break label$20
         }
         $17 = $3 + $21 | 0;
         if (HEAP32[$17 >> 2] != ($4 | 0)) {
          break label$20
         }
         if ((HEAP32[$3 >> 2] ^ HEAP32[$7 >> 2]) < 0) {
          break label$20
         }
         HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + $48;
         $31 = $3 + $22 | 0;
         $32 = HEAP32[$31 >> 2];
         $18 = HEAP32[$3 + 4 >> 2];
         HEAP32[$32 >> 2] = $18;
         HEAP32[$18 + 4 >> 2] = $32;
         HEAP32[$3 + 4 >> 2] = $49;
         HEAP32[$17 >> 2] = 0;
         HEAP32[$31 >> 2] = $50;
         $3 = $3 + $47 | 0;
         HEAP32[$3 >> 2] = HEAP32[$3 >> 2] | $51;
         $5 = $5 + $9 | 0;
         $3 = $6 & $12;
         $7 = Math_imul($3 - $6 | 0, $9) + $7 | 0;
         $6 = $3;
         $4 = $4 + $11 | 0;
         if ($4 >>> 0 < $52 >>> 0) {
          continue
         }
         break label$19;
        }
        break;
       };
       $3 = $6;
      }
      HEAP32[$7 + $21 >> 2] = $4;
      HEAP32[$7 + 24 >> 2] = HEAP32[$7 + 24 >> 2] & $53;
      label$22 : {
       label$23 : {
        if ($4 >>> 0 > $22 >>> 0) {
         break label$23
        }
        $6 = $3 & $12;
        $3 = Math_imul($6 - $3 | 0, $9) + $7 | 0;
        $5 = $4 + $11 | 0;
        $6 = $3 + Math_imul(($6 ^ $11 << $5) - $6 | 0, $9) | 0;
        if ((HEAP32[$6 + 24 >> 2] & $25) != ($26 | 0)) {
         break label$23
        }
        if (($5 | 0) != HEAP32[$6 + $21 >> 2]) {
         break label$23
        }
        if ((HEAP32[$6 >> 2] ^ HEAP32[$3 >> 2]) < 0) {
         break label$23
        }
        $5 = (Math_imul($4, $9) + $0 | 0) + ($10 << $27) | 0;
        $6 = $5 + $54 | 0;
        $3 = HEAP32[$6 >> 2];
        $18 = $6;
        $6 = $7 + $28 | 0;
        HEAP32[$18 >> 2] = $6;
        $5 = $5 + $29 | 0;
        break label$22;
       }
       $3 = ((Math_imul($4, $9) + $0 | 0) + ($10 << $27) | 0) + $29 | 0;
       $5 = HEAP32[$3 >> 2];
       $6 = $7 + $28 | 0;
       HEAP32[$5 + 4 >> 2] = $6;
      }
      HEAP32[$7 + 4 >> 2] = $5;
      HEAP32[$3 >> 2] = $6;
      HEAP32[$7 + $22 >> 2] = $3;
      $3 = (Math_imul($4, $9) + $0 | 0) + $44 | 0;
      HEAP32[$3 >> 2] = HEAP32[$3 >> 2] + $11;
      if (($8 + 8 | 0) != ($15 | 0)) {
       break label$6
      }
     }
     global$0 = $8 + 16 | 0;
     return;
    }
    $6 = 1;
    $3 = 1;
    continue;
   }
   $16 = 0 - $3 | 0;
   $6 = 0;
   $3 = 1;
   continue;
  };
 }
 
 function drain_all_pages() {
  var $0 = 0;
  label$1 : {
   if (!HEAP32[20654]) {
    break label$1
   }
   label$2 : {
    label$3 : {
     label$4 : {
      if (mutex_trylock(16848)) {
       $0 = 141272;
       if (!$0) {
        break label$3
       }
       while (1) {
        if (HEAP32[HEAP32[$0 + 28 >> 2] >> 2] ? HEAP32[$0 + 48 >> 2] : 0) {
         break label$4
        }
        $0 = HEAP32[$0 + 24 >> 2] + 516 >>> 0 > $0 >>> 0 ? $0 + 516 | 0 : 0;
        if ($0) {
         continue
        }
        break;
       };
       break label$3;
      }
      break label$1;
     }
     $0 = HEAP32[20710] | 1;
     break label$2;
    }
    $0 = HEAP32[20710] & -2;
   }
   HEAP32[4223] = 16892;
   HEAP32[4222] = -32;
   HEAP32[20710] = $0;
   HEAP32[4225] = 88;
   HEAP32[4224] = 16892;
   queue_work_on(0, HEAP32[20654], 16888);
   __flush_work();
   mutex_unlock(16848);
  }
 }
 
 function drain_local_pages_wq($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $0 = 141272;
  if ($0) {
   while (1) {
    label$3 : {
     if (!HEAP32[$0 + 48 >> 2]) {
      break label$3
     }
     $1 = HEAP32[$0 + 28 >> 2];
     $2 = HEAP32[$1 >> 2];
     if (!$2) {
      break label$3
     }
     free_pcppages_bulk($0, $2, $1);
    }
    $0 = HEAP32[$0 + 24 >> 2] + 516 >>> 0 > $0 >>> 0 ? $0 + 516 | 0 : 0;
    if ($0) {
     continue
    }
    break;
   }
  }
 }
 
 function free_unref_page($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $1 = ($0 - HEAP32[20646] | 0) / 36 | 0;
  $2 = $0 + 12 | 0;
  if (HEAPU8[$2 | 0] & 3) {
   HEAP32[$2 >> 2] = 0
  }
  $2 = HEAP32[$0 >> 2];
  HEAP32[$0 >> 2] = $2 & -4194304;
  $2 = Math_imul($2 >>> 31 | 0, 516);
  $1 = $1 - (HEAP32[$2 + 141308 >> 2] & -1024) | 0;
  $1 = HEAP32[HEAP32[$2 + 141304 >> 2] + ($1 >>> 11 & 2097148) >> 2] >>> 29 - ($1 >>> 8 & 28) & 7;
  HEAP32[$0 + 16 >> 2] = $1;
  $3 = ($1 >>> 0 > 2 ? 1 : $1) << 3;
  $1 = HEAP32[$2 + 141300 >> 2];
  $4 = ($3 + $1 | 0) + 12 | 0;
  $5 = HEAP32[$4 >> 2];
  $3 = $0 + 4 | 0;
  HEAP32[$5 + 4 >> 2] = $3;
  HEAP32[$4 >> 2] = $3;
  $3 = HEAP32[$1 >> 2] + 1 | 0;
  HEAP32[$1 >> 2] = $3;
  HEAP32[$0 + 4 >> 2] = $5;
  HEAP32[$0 + 8 >> 2] = $4;
  if (($3 | 0) >= HEAP32[$1 + 4 >> 2]) {
   free_pcppages_bulk($2 + 141272 | 0, HEAP32[$1 + 8 >> 2], $1)
  }
 }
 
 function free_unref_page_list($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $1 = HEAP32[$0 >> 2];
  if (($0 | 0) != ($1 | 0)) {
   $6 = HEAP32[20646];
   while (1) {
    $2 = $1 + -4 | 0;
    $4 = ($2 - $6 | 0) / 36 | 0;
    $7 = HEAP32[$1 >> 2];
    if (HEAPU8[$1 + 8 | 0] & 3) {
     HEAP32[$1 + 8 >> 2] = 0
    }
    $5 = $2;
    $2 = HEAP32[$2 >> 2];
    HEAP32[$5 >> 2] = $2 & -4194304;
    $3 = Math_imul($2 >>> 31 | 0, 516);
    $2 = $4 - (HEAP32[$3 + 141308 >> 2] & -1024) | 0;
    $3 = HEAP32[HEAP32[$3 + 141304 >> 2] + ($2 >>> 11 & 2097148) >> 2];
    HEAP32[$1 + 16 >> 2] = $4;
    HEAP32[$1 + 12 >> 2] = $3 >>> 29 - ($2 >>> 8 & 28) & 7;
    $1 = $7;
    if (($1 | 0) != ($0 | 0)) {
     continue
    }
    break;
   };
   $1 = HEAP32[$0 >> 2];
  }
  if (($0 | 0) != ($1 | 0)) {
   while (1) {
    HEAP32[$1 + 16 >> 2] = 0;
    $7 = HEAP32[$1 >> 2];
    $2 = HEAP32[$1 + 12 >> 2];
    $5 = (($2 | 0) > 2 ? 1 : $2) << 3;
    $6 = Math_imul(HEAP32[$1 + -4 >> 2] >>> 31 | 0, 516);
    $2 = HEAP32[$6 + 141300 >> 2];
    $4 = ($5 + $2 | 0) + 12 | 0;
    $3 = HEAP32[$4 >> 2];
    HEAP32[$3 + 4 >> 2] = $1;
    HEAP32[$4 >> 2] = $1;
    $5 = HEAP32[$2 >> 2] + 1 | 0;
    HEAP32[$2 >> 2] = $5;
    HEAP32[$1 >> 2] = $3;
    HEAP32[$1 + 4 >> 2] = $4;
    if (($5 | 0) >= HEAP32[$2 + 4 >> 2]) {
     free_pcppages_bulk($6 + 141272 | 0, HEAP32[$2 + 8 >> 2], $2)
    }
    $1 = $7;
    if (($1 | 0) != ($0 | 0)) {
     continue
    }
    break;
   }
  }
 }
 
 function zone_watermark_ok_safe($0, $1, $2, $3) {
  var $4 = 0, $5 = 0;
  $5 = 1;
  $4 = HEAP32[$0 + 468 >> 2];
  if ($4 >>> 0 < HEAPU32[$0 + 460 >> 2]) {
   $4 = HEAP32[$0 + 468 >> 2]
  }
  label$2 : {
   label$3 : {
    if (((($4 + (-1 << $1) | 0) + 1 | 0) - HEAP32[$0 + 12 >> 2] | 0) <= (HEAP32[(($3 << 2) + $0 | 0) + 16 >> 2] + $2 | 0)) {
     break label$3
    }
    if (!$1) {
     break label$2
    }
    if (($1 | 0) > 10) {
     break label$3
    }
    $0 = (Math_imul($1, 36) + $0 | 0) + 60 | 0;
    while (1) {
     if (HEAP32[$0 + 32 >> 2]) {
      if (HEAP32[$0 >> 2] != ($0 | 0)) {
       break label$2
      }
      $2 = $0 + 8 | 0;
      if (HEAP32[$2 >> 2] != ($2 | 0)) {
       break label$2
      }
      $2 = $0 + 16 | 0;
      if (HEAP32[$2 >> 2] != ($2 | 0)) {
       break label$2
      }
     }
     $0 = $0 + 36 | 0;
     $1 = $1 + 1 | 0;
     if (($1 | 0) < 11) {
      continue
     }
     break;
    };
   }
   $5 = 0;
  }
  return $5;
 }
 
 function warn_alloc($0, $1) {
  var $2 = 0, $3 = 0;
  $2 = global$0 - 48 | 0;
  global$0 = $2;
  HEAP32[$2 + 44 >> 2] = $0;
  label$1 : {
   if ($0 & 512) {
    break label$1
   }
   if (!___ratelimit(16972, 26560)) {
    break label$1
   }
   $3 = HEAP32[2];
   HEAP32[$2 + 20 >> 2] = 0;
   HEAP32[$2 + 16 >> 2] = 0;
   HEAP32[$2 + 28 >> 2] = $1;
   HEAP32[$2 + 32 >> 2] = 28067;
   HEAP32[$2 + 8 >> 2] = $0;
   HEAP32[$2 >> 2] = $3 + 700;
   HEAP32[$2 + 36 >> 2] = $2 + 28;
   HEAP32[$2 + 12 >> 2] = $2 + 44;
   HEAP32[$2 + 4 >> 2] = $2 + 32;
   printk(26571, $2);
   dump_stack();
   $0 = HEAP32[$2 + 44 >> 2];
   if (!___ratelimit(16996, 27998)) {
    break label$1
   }
   $1 = 1;
   label$2 : {
    if ($0 & 65536) {
     break label$2
    }
    $3 = HEAP32[2];
    if (HEAPU16[$3 + 12 >> 1] & 2052 ? 0 : !HEAP32[HEAP32[$3 + 732 >> 2] + 368 >> 2]) {
     break label$2
    }
    $1 = 0;
   }
   show_mem($0 >>> 21 & (HEAP32[1] & 2096896 ? 0 : $1), 0);
  }
  global$0 = $2 + 48 | 0;
 }
 
 function __alloc_pages_nodemask($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0;
  $3 = global$0 + -64 | 0;
  global$0 = $3;
  $2 = $3 + 24 | 0;
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  $2 = $3 + 16 | 0;
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  HEAP32[$3 + 8 >> 2] = 0;
  HEAP32[$3 + 12 >> 2] = 0;
  label$1 : {
   if ($1 >>> 0 <= 10) {
    HEAP32[$3 + 12 >> 2] = 0;
    HEAP32[$3 + 8 >> 2] = 142304;
    $0 = HEAP32[4221] & $0;
    HEAP8[$3 + 28 | 0] = $0 >>> 8 & 1;
    $4 = 1024 >>> ($0 & 15) & 1;
    HEAP32[$3 + 24 >> 2] = $4;
    $7 = HEAP32[20703] ? 0 : $0 >>> 3 & 3;
    HEAP32[$3 + 20 >> 2] = $7;
    $8 = $3;
    if (HEAPU32[35577] <= $4 >>> 0 ? 1 : 0) {
     $2 = 142304
    } else {
     $2 = __next_zones_zonelist(142304, $4, 0)
    }
    HEAP32[$8 + 16 >> 2] = $2;
    $2 = get_page_from_freelist($0, $1, 1, $3 + 8 | 0);
    if ($2) {
     break label$1
    }
    $8 = HEAP32[2];
    $2 = HEAP32[$8 + 12 >> 2];
    label$5 : {
     if (!($2 & 524288)) {
      $0 = $2 & 262144 ? $0 & -129 : $0;
      break label$5;
     }
     $0 = $0 & -193;
    }
    HEAP8[$3 + 28 | 0] = 0;
    $5 = ($0 & 2621440) == 2621440 ? $0 & -524289 : $0;
    $2 = $5 & 32;
    label$7 : {
     if (!($5 & 524288)) {
      $10 = $2 | 64;
      $11 = $10;
      if (HEAP32[$8 + 24 >> 2] > 99) {
       break label$7
      }
      $11 = HEAP32[1] & 2096896 ? $10 : $2 | 80;
      break label$7;
     }
     $11 = ($2 | $5 >>> 12 & 16) ^ 16;
    }
    $8 = $11;
    $16 = $0 & 2097152;
    $0 = $3 + 16 | 0;
    if (HEAPU32[35577] <= $4 >>> 0) {
     $11 = 142304
    } else {
     $11 = __next_zones_zonelist(142304, $4, 0)
    }
    HEAP32[$0 >> 2] = $11;
    label$11 : {
     label$12 : {
      label$13 : {
       label$14 : {
        label$15 : {
         label$16 : {
          if (HEAP32[$11 >> 2]) {
           $17 = $5 & 4194304;
           if (!$17) {
            break label$13
           }
           if (HEAPU32[35577] > $4 >>> 0) {
            break label$16
           }
           $2 = 142304;
           $0 = HEAP32[35576];
           if (!$0) {
            break label$13
           }
           break label$14;
          }
          if ($5 & 2048) {
           break label$15
          }
          break label$11;
         }
         $2 = __next_zones_zonelist(142304, $4, 0);
         $0 = HEAP32[$2 >> 2];
         if ($0) {
          break label$14
         }
         break label$13;
        }
        $0 = 1;
        break label$12;
       }
       while (1) {
        if (($6 | 0) != HEAP32[$0 + 24 >> 2]) {
         wakeup_kswapd($0, $1, $4);
         $6 = HEAP32[$0 + 24 >> 2];
        }
        $0 = $2 + 8 | 0;
        if (HEAPU32[$2 + 12 >> 2] <= $4 >>> 0) {
         $2 = $0;
         $0 = HEAP32[$0 >> 2];
         if ($0) {
          continue
         }
         break label$13;
        }
        $2 = __next_zones_zonelist($0, $4, 0);
        $0 = HEAP32[$2 >> 2];
        if ($0) {
         continue
        }
        break;
       };
      }
      $2 = get_page_from_freelist($5, $1, $8, $3 + 8 | 0);
      if ($2) {
       break label$1
      }
      $6 = 0;
      label$21 : {
       if (!$16) {
        break label$21
       }
       if ($1 >>> 0 <= 3) {
        if (!$1) {
         break label$21
        }
        if (($7 | 0) == 1) {
         break label$21
        }
       }
       if ($5 & 73728) {
        break label$21
       }
       if (HEAP32[1] & 256) {
        if (HEAPU8[HEAP32[2] + 13 | 0] & 8) {
         break label$21
        }
       }
      }
      $0 = 0;
     }
     label$24 : while (1) {
      if (!$0) {
       $18 = $1 >>> 0 > 3;
       $24 = $18 & !($5 & 1024) | ($5 & 4096) >>> 12;
       $25 = $5 & 263168;
       $26 = $5 & 8192;
       $21 = $5 & 65536;
       $27 = $21 >>> 16 | 0;
       $13 = $5 & 2048;
       $28 = $13 >>> 11 | 0;
       $29 = $5 & -2228225 | 131072;
       $30 = (-1 << $1) + 1 | 0;
       $31 = ($1 | 0) > 10;
       $32 = Math_imul($1, 9) + 15 << 2;
       $33 = $1 + -1 >>> 0 > 2;
       $34 = $3 + 60 | 0;
       $22 = $3 + 52 | 0;
       while (1) {
        label$28 : {
         if (!$17) {
          break label$28
         }
         $2 = 142304;
         label$29 : {
          if (HEAPU32[35577] <= $4 >>> 0) {
           $0 = HEAP32[35576];
           if (!$0) {
            break label$28
           }
           break label$29;
          }
          $2 = __next_zones_zonelist(142304, $4, 0);
          $0 = HEAP32[$2 >> 2];
          if (!$0) {
           break label$28
          }
         }
         $9 = 0;
         while (1) {
          if (($9 | 0) != HEAP32[$0 + 24 >> 2]) {
           wakeup_kswapd($0, $1, $4);
           $9 = HEAP32[$0 + 24 >> 2];
          }
          $0 = $2 + 8 | 0;
          label$33 : {
           if (HEAPU32[$2 + 12 >> 2] <= $4 >>> 0) {
            $2 = $0;
            $0 = HEAP32[$0 >> 2];
            if ($0) {
             continue
            }
            break label$33;
           }
           $2 = __next_zones_zonelist($0, $4, 0);
           $0 = HEAP32[$2 >> 2];
           if ($0) {
            continue
           }
          }
          break;
         };
        }
        label$35 : {
         label$36 : {
          label$37 : {
           if ($21) {
            break label$37
           }
           $0 = 4;
           if ($26) {
            break label$36
           }
           if (HEAP32[1] & 256) {
            if (HEAPU8[HEAP32[2] + 13 | 0] & 8) {
             break label$36
            }
           }
           if (HEAP32[1] & 2096896) {
            break label$37
           }
           $9 = HEAP32[2];
           if (HEAPU8[$9 + 13 | 0] & 8) {
            break label$36
           }
           $0 = 8;
           if (HEAP32[HEAP32[$9 + 732 >> 2] + 368 >> 2]) {
            break label$36
           }
          }
          $0 = $8;
          if ($0 & 64) {
           break label$35
          }
         }
         HEAP32[$3 + 12 >> 2] = 0;
         $11 = 142304;
         if (HEAPU32[35577] > $4 >>> 0) {
          $11 = __next_zones_zonelist(142304, $4, 0)
         }
         HEAP32[$3 + 16 >> 2] = $11;
         $8 = $0;
        }
        $2 = get_page_from_freelist($5, $1, $8, $3 + 8 | 0);
        if ($2) {
         break label$1
        }
        label$40 : {
         label$41 : {
          label$42 : {
           label$43 : {
            label$44 : {
             label$45 : {
              if (!$16) {
               break label$45
              }
              if (HEAPU8[HEAP32[2] + 13 | 0] & 8) {
               break label$45
              }
              _cond_resched();
              $0 = HEAP32[2];
              $9 = HEAP32[$0 + 12 >> 2];
              HEAP32[$0 + 12 >> 2] = $9 | 2048;
              HEAP32[$3 + 32 >> 2] = 0;
              HEAP32[$0 + 804 >> 2] = $3 + 32;
              $10 = try_to_free_pages($1, $5);
              $0 = HEAP32[2];
              HEAP32[$0 + 804 >> 2] = 0;
              HEAP32[$0 + 12 >> 2] = HEAP32[$0 + 12 >> 2] & -2049 | $9 & 2048;
              _cond_resched();
              if ($10) {
               $2 = get_page_from_freelist($5, $1, $8, $3 + 8 | 0);
               if ($2) {
                break label$1
               }
               unreserve_highatomic_pageblock($3 + 8 | 0, 0);
               drain_all_pages();
               $2 = get_page_from_freelist($5, $1, $8, $3 + 8 | 0);
               if ($2) {
                break label$1
               }
              }
              if ($24) {
               break label$44
              }
              label$47 : {
               if (!$18) {
                $9 = 0;
                if ($10) {
                 break label$47
                }
               }
               $9 = $6 + 1 | 0;
               if (($6 | 0) < 16) {
                break label$47
               }
               $6 = $9;
               if (unreserve_highatomic_pageblock($3 + 8 | 0, 1)) {
                continue
               }
               break label$40;
              }
              $7 = 142304;
              if (HEAPU32[35577] <= $4 >>> 0) {
               break label$41
              }
              $7 = __next_zones_zonelist(142304, $4, 0);
              break label$41;
             }
             $9 = $6;
             if ($13) {
              break label$43
             }
             break label$11;
            }
            $9 = $6;
            if ($13) {
             break label$42
            }
            break label$11;
           }
           $0 = 1;
           continue label$24;
          }
          $0 = 1;
          continue label$24;
         }
         $14 = 0;
         label$50 : {
          $2 = HEAP32[$7 >> 2];
          if (!$2) {
           break label$50
          }
          $19 = $8 & 8;
          $20 = $8 & 32;
          $15 = $8 & 24;
          label$51 : {
           while (1) {
            $12 = HEAP32[$11 + 4 >> 2];
            $0 = HEAP32[$2 >> 2];
            $6 = $20 ? $0 - (($0 | 0) / 2 | 0) | 0 : $0;
            $23 = HEAP32[$2 + 480 >> 2] + HEAP32[$2 + 484 >> 2] | 0;
            $0 = HEAP32[$2 + 468 >> 2] + ($23 + $30 | 0) | 0;
            label$54 : {
             if ($15) {
              if (!$19) {
               $6 = $6 - (($6 | 0) / 4 | 0) | 0;
               break label$54;
              }
              $6 = $6 - (($6 | 0) / 2 | 0) | 0;
              break label$54;
             }
             $0 = $0 - HEAP32[$2 + 12 >> 2] | 0;
            }
            label$57 : {
             if (($0 | 0) <= (HEAP32[(($12 << 2) + $2 | 0) + 16 >> 2] + $6 | 0)) {
              break label$57
             }
             if (!$1) {
              break label$51
             }
             if ($31) {
              break label$57
             }
             $0 = $2 + $32 | 0;
             $6 = $1;
             while (1) {
              label$59 : {
               if (!HEAP32[$0 + 32 >> 2]) {
                break label$59
               }
               if (HEAP32[$0 >> 2] != ($0 | 0)) {
                break label$51
               }
               $12 = $0 + 8 | 0;
               if (HEAP32[$12 >> 2] != ($12 | 0)) {
                break label$51
               }
               $12 = $0 + 16 | 0;
               if (HEAP32[$12 >> 2] != ($12 | 0)) {
                break label$51
               }
               if (!$15) {
                break label$59
               }
               $12 = $0 + 24 | 0;
               if (HEAP32[$12 >> 2] != ($12 | 0)) {
                break label$51
               }
              }
              $0 = $0 + 36 | 0;
              $6 = $6 + 1 | 0;
              if (($6 | 0) < 11) {
               continue
              }
              break;
             };
            }
            $0 = $7 + 8 | 0;
            label$60 : {
             if (HEAPU32[$7 + 12 >> 2] <= $4 >>> 0) {
              $7 = $0;
              $2 = HEAP32[$0 >> 2];
              if ($2) {
               continue
              }
              break label$60;
             }
             $7 = __next_zones_zonelist($0, $4, 0);
             $2 = HEAP32[$7 >> 2];
             if ($2) {
              continue
             }
            }
            break;
           };
           break label$50;
          }
          $14 = 1;
          if ($10) {
           break label$50
          }
          if (HEAP32[$2 + 492 >> 2] << 1 >>> 0 <= $23 >>> 0) {
           break label$50
          }
          congestion_wait();
          $6 = $9;
          continue;
         }
         label$62 : {
          if (!(HEAPU8[HEAP32[2] + 12 | 0] & 32)) {
           _cond_resched();
           break label$62;
          }
          schedule_timeout_uninterruptible();
         }
         $6 = $9;
         if ($14) {
          continue
         }
        }
        label$64 : {
         if ($33) {
          break label$64
         }
         if (!$10) {
          break label$64
         }
         $10 = 142304;
         label$65 : {
          if (HEAPU32[35577] <= $4 >>> 0) {
           $0 = HEAP32[35576];
           if (!$0) {
            break label$64
           }
           break label$65;
          }
          $10 = __next_zones_zonelist(142304, $4, 0);
          $0 = HEAP32[$10 >> 2];
          if (!$0) {
           break label$64
          }
         }
         $14 = $8 & 8;
         $15 = $8 & 32;
         $19 = $8 & 24;
         while (1) {
          $20 = HEAP32[$11 + 4 >> 2];
          $2 = HEAP32[$0 + 468 >> 2];
          $7 = HEAP32[$0 >> 2];
          $7 = $15 ? $7 - (($7 | 0) / 2 | 0) | 0 : $7;
          label$69 : {
           if ($19) {
            if (!$14) {
             $7 = $7 - (($7 | 0) / 4 | 0) | 0;
             break label$69;
            }
            $7 = $7 - (($7 | 0) / 2 | 0) | 0;
            break label$69;
           }
           $2 = $2 - HEAP32[$0 + 12 >> 2] | 0;
          }
          if (($2 | 0) <= (HEAP32[(($20 << 2) + $0 | 0) + 16 >> 2] + $7 | 0)) {
           $0 = $10 + 8 | 0;
           if (HEAPU32[$10 + 12 >> 2] <= $4 >>> 0) {
            $10 = $0;
            $0 = HEAP32[$0 >> 2];
            if ($0) {
             continue
            }
            break label$64;
           }
           $10 = __next_zones_zonelist($0, $4, 0);
           $0 = HEAP32[$10 >> 2];
           if ($0) {
            continue
           }
           break label$64;
          }
          break;
         };
         $6 = $9;
         continue;
        }
        HEAP32[$34 >> 2] = 0;
        HEAP32[$22 >> 2] = 0;
        HEAP32[$22 + 4 >> 2] = 0;
        HEAP32[$3 + 48 >> 2] = $1;
        HEAP32[$3 + 44 >> 2] = $5;
        HEAP32[$3 + 40 >> 2] = 0;
        HEAP32[$3 + 36 >> 2] = 0;
        HEAP32[$3 + 32 >> 2] = 142304;
        label$74 : {
         label$75 : {
          label$76 : {
           if (mutex_trylock(17024)) {
            $0 = 0;
            $2 = get_page_from_freelist($29, $1, 66, $3 + 8 | 0);
            if (!$2) {
             break label$76
            }
            break label$75;
           }
           $0 = 1;
           schedule_timeout_uninterruptible();
           break label$74;
          }
          label$78 : {
           if (!$18) {
            $2 = 0;
            if (HEAP32[HEAP32[2] + 12 >> 2] & 512 | $25) {
             break label$75
            }
            $0 = out_of_memory($3 + 32 | 0);
            if ($13) {
             break label$78
            }
            $0 = $0 | $28;
           }
           mutex_unlock(17024);
           break label$74;
          }
          $0 = 1;
          $2 = get_page_from_freelist($5, $1, 68, $3 + 8 | 0);
          if ($2) {
           break label$75
          }
          $2 = get_page_from_freelist($5, $1, 4, $3 + 8 | 0);
          mutex_unlock(17024);
          if (!$2) {
           break label$74
          }
          break label$1;
         }
         mutex_unlock(17024);
         if ($2) {
          break label$1
         }
        }
        if (!(($8 | 0) == 8 | $27 ? !!HEAP32[HEAP32[HEAP32[2] + 732 >> 2] + 368 >> 2] : 0)) {
         $6 = 0;
         if ($0) {
          continue
         }
        }
        break;
       };
       if (!$13) {
        break label$11
       }
       $0 = 1;
       continue;
      }
      if (!$16) {
       break label$11
      }
      $2 = get_page_from_freelist($5, $1, 80, $3 + 8 | 0);
      if ($2) {
       break label$1
      }
      $2 = get_page_from_freelist($5, $1, 16, $3 + 8 | 0);
      if ($2) {
       break label$1
      }
      $17 = $5 & 4194304;
      _cond_resched();
      $6 = $9;
      $0 = 0;
      continue;
     };
    }
    HEAP32[$3 >> 2] = $1;
    warn_alloc($5, $3);
   }
   $2 = 0;
  }
  global$0 = $3 - -64 | 0;
  return $2;
 }
 
 function get_page_from_freelist($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0, $35 = 0, $36 = 0, $37 = 0, $38 = 0, $39 = 0, $40 = 0, $41 = 0, $42 = 0, $43 = 0, $44 = 0, $45 = 0, $46 = 0, $47 = 0, $48 = 0, $49 = 0, $50 = 0, $51 = 0, $52 = 0, $53 = 0, $54 = 0, $55 = 0, $56 = 0, $57 = 0, $58 = 0, $59 = 0, $60 = 0, $61 = 0, $62 = 0, $63 = 0, $64 = 0, $65 = 0, $66 = 0, $67 = 0, $68 = 0, $69 = 0, $70 = 0, $71 = 0, $72 = 0, $73 = 0, $74 = 0, $75 = 0, $76 = 0, $77 = 0, $78 = 0, $79 = 0, $80 = 0, $81 = 0, $82 = 0, $83 = 0, $84 = 0, $85 = 0, $86 = 0, $87 = 0;
  $16 = global$0 - 16 | 0;
  global$0 = $16;
  label$1 : {
   label$2 : {
    $19 = HEAP32[$3 + 8 >> 2];
    $8 = HEAP32[$19 >> 2];
    if (!$8) {
     break label$2
    }
    $42 = $2 & 16;
    $43 = $42 >>> 4 & $1 >>> 0 < 11;
    $24 = 1 << $1;
    $70 = $2 & 4;
    $71 = $2 & 8;
    $72 = $2 & 32;
    $44 = $2 & 24;
    $27 = Math_imul($1, 9);
    $45 = $27 + 15 | 0;
    $28 = -1 << $1;
    $73 = $28 ^ -1;
    $74 = $3 + 20 | 0;
    $75 = ($2 & 3) << 2;
    $76 = $27 + 12 << 2;
    label$3 : while (1) {
     label$4 : {
      label$6 : {
       label$7 : {
        label$8 : {
         label$9 : {
          label$10 : {
           label$11 : {
            label$12 : {
             label$13 : {
              label$14 : {
               label$15 : {
                switch ($4 | 0) {
                case 1:
                 if ($1 >>> 0 > 10) {
                  break label$14
                 }
                 $4 = 0;
                 break label$9;
                default:
                 if (($1 | 0) > 10) {
                  break label$4
                 }
                 $29 = HEAP32[20703];
                 $46 = 10;
                 $17 = 1;
                 break label$11;
                case 0:
                 break label$15;
                };
               }
               label$18 : {
                label$19 : {
                 label$20 : {
                  if (HEAPU8[$74 | 0]) {
                   $5 = HEAP32[$8 + 24 >> 2];
                   if (($5 | 0) == ($77 | 0)) {
                    break label$4
                   }
                   if (!node_dirty_ok($5)) {
                    break label$20
                   }
                  }
                  $6 = HEAP32[HEAP32[$3 + 8 >> 2] + 4 >> 2];
                  $5 = HEAP32[$8 + $75 >> 2];
                  $4 = HEAP32[$8 + 468 >> 2];
                  label$22 : {
                   label$23 : {
                    if ($1) {
                     break label$23
                    }
                    if ($4 >>> 0 <= HEAP32[(($6 << 2) + $8 | 0) + 16 >> 2] + $5 >>> 0) {
                     break label$23
                    }
                    $12 = HEAP32[$3 + 12 >> 2];
                    break label$22;
                   }
                   $5 = $72 ? $5 - (($5 | 0) / 2 | 0) | 0 : $5;
                   $4 = $4 - $73 | 0;
                   label$25 : {
                    if ($44) {
                     if (!$71) {
                      $5 = $5 - (($5 | 0) / 4 | 0) | 0;
                      break label$25;
                     }
                     $5 = $5 - (($5 | 0) / 2 | 0) | 0;
                     break label$25;
                    }
                    $4 = $4 - HEAP32[$8 + 12 >> 2] | 0;
                   }
                   $5 = HEAP32[(($6 << 2) + $8 | 0) + 16 >> 2] + $5 | 0;
                   label$28 : {
                    label$29 : {
                     label$30 : {
                      if (!$1) {
                       break label$30
                      }
                      if (($4 | 0) <= ($5 | 0)) {
                       break label$30
                      }
                      if (($1 | 0) > 10) {
                       break label$29
                      }
                      $4 = ($45 << 2) + $8 | 0;
                      $5 = $1;
                      while (1) {
                       label$32 : {
                        if (!HEAP32[$4 + 32 >> 2]) {
                         break label$32
                        }
                        if (HEAP32[$4 >> 2] != ($4 | 0)) {
                         break label$28
                        }
                        $6 = $4 + 8 | 0;
                        if (HEAP32[$6 >> 2] != ($6 | 0)) {
                         break label$28
                        }
                        $6 = $4 + 16 | 0;
                        if (HEAP32[$6 >> 2] != ($6 | 0)) {
                         break label$28
                        }
                        if (!$44) {
                         break label$32
                        }
                        $6 = $4 + 24 | 0;
                        if (HEAP32[$6 >> 2] != ($6 | 0)) {
                         break label$28
                        }
                       }
                       $4 = $4 + 36 | 0;
                       $5 = $5 + 1 | 0;
                       if (($5 | 0) < 11) {
                        continue
                       }
                       break;
                      };
                      break label$29;
                     }
                     if (($4 | 0) > ($5 | 0)) {
                      break label$28
                     }
                    }
                    if (!$70) {
                     break label$4
                    }
                   }
                   $12 = HEAP32[$3 + 12 >> 2];
                   if (!$1) {
                    break label$22
                   }
                   $78 = $12 | 2;
                   $30 = $12 << 3;
                   $79 = $30 + 24 | 0;
                   $80 = ($27 << 2) + $8 | 0;
                   $5 = $12 << 4;
                   $81 = $5 + 26500 | 0;
                   $82 = $8 + $76 | 0;
                   $31 = ($45 << 2) + $8 | 0;
                   $83 = $5 + 26496 | 0;
                   if (!$43) {
                    break label$18
                   }
                   $17 = 0;
                   break label$12;
                  }
                  $20 = HEAP32[$8 + 28 >> 2];
                  $14 = $12 << 3;
                  $5 = $20 + $14 | 0;
                  $22 = $5 + 12 | 0;
                  $4 = HEAP32[$22 >> 2];
                  if (($4 | 0) != ($22 | 0)) {
                   break label$7
                  }
                  $47 = HEAP32[$20 + 8 >> 2];
                  if (!$47) {
                   break label$19
                  }
                  $23 = $8 + 420 | 0;
                  $48 = $23 + $14 | 0;
                  $32 = $8 + 384 | 0;
                  $49 = $32 + $14 | 0;
                  $33 = $8 + 348 | 0;
                  $50 = $33 + $14 | 0;
                  $34 = $8 + 312 | 0;
                  $51 = $34 + $14 | 0;
                  $35 = $8 + 276 | 0;
                  $52 = $35 + $14 | 0;
                  $36 = $8 + 240 | 0;
                  $53 = $36 + $14 | 0;
                  $37 = $8 + 204 | 0;
                  $54 = $37 + $14 | 0;
                  $38 = $8 + 168 | 0;
                  $55 = $38 + $14 | 0;
                  $39 = $8 + 132 | 0;
                  $56 = $39 + $14 | 0;
                  $40 = $8 + 96 | 0;
                  $57 = $40 + $14 | 0;
                  $41 = $8 + 60 | 0;
                  $25 = $41 + $14 | 0;
                  $58 = $8 + 436 | 0;
                  $59 = $8 + 400 | 0;
                  $60 = $8 + 364 | 0;
                  $61 = $8 + 328 | 0;
                  $62 = $8 + 292 | 0;
                  $63 = $8 + 256 | 0;
                  $64 = $8 + 220 | 0;
                  $65 = $8 + 184 | 0;
                  $66 = $8 + 148 | 0;
                  $67 = $8 + 112 | 0;
                  $68 = $8 + 76 | 0;
                  $84 = $12 | 2;
                  $85 = $14 + 24 | 0;
                  $4 = $12 << 4;
                  $86 = $4 + 26500 | 0;
                  $87 = $4 + 26496 | 0;
                  $69 = $5 + 16 | 0;
                  $21 = 0;
                  $26 = 0;
                  break label$13;
                 }
                 $77 = HEAP32[$8 + 24 >> 2];
                 break label$4;
                }
                $26 = 0;
                $21 = 0;
                break label$8;
               }
               $4 = 1;
               continue;
              }
              $4 = 2;
              continue;
             }
             $17 = 3;
             break label$10;
            }
            $4 = 1;
            break label$9;
           }
           $4 = 1;
           break label$9;
          }
          $4 = 1;
         }
         while (1) {
          label$34 : {
           label$35 : {
            label$36 : {
             label$37 : {
              label$38 : {
               label$39 : {
                label$40 : {
                 label$41 : {
                  label$42 : {
                   label$43 : {
                    label$44 : {
                     label$45 : {
                      if (!$4) {
                       $4 = $80;
                       $6 = $31;
                       $5 = $1;
                       label$48 : {
                        while (1) {
                         $9 = $6 + $30 | 0;
                         $7 = HEAP32[$9 >> 2];
                         if (($7 | 0) != ($9 | 0)) {
                          $10 = $7 + -4 | 0;
                          if ($10) {
                           break label$48
                          }
                         }
                         $4 = $4 + 36 | 0;
                         $6 = $6 + 36 | 0;
                         $5 = $5 + 1 | 0;
                         if ($5 >>> 0 < 11) {
                          continue
                         }
                         break;
                        };
                        $4 = 2;
                        continue label$3;
                       }
                       $7 = HEAP32[$10 + 4 >> 2];
                       $9 = HEAP32[$10 + 8 >> 2];
                       HEAP32[$7 + 4 >> 2] = $9;
                       HEAP32[$9 >> 2] = $7;
                       HEAP32[$10 + 20 >> 2] = 0;
                       HEAP32[$10 + 24 >> 2] = HEAP32[$10 + 24 >> 2] | 128;
                       HEAP32[$10 + 4 >> 2] = 256;
                       HEAP32[$10 + 8 >> 2] = 512;
                       $6 = $6 + 32 | 0;
                       HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + -1;
                       if (($5 | 0) > ($1 | 0)) {
                        $9 = 1 << $5;
                        while (1) {
                         $7 = $4 + $79 | 0;
                         $11 = HEAP32[$7 >> 2];
                         $9 = $9 >>> 1 | 0;
                         $6 = Math_imul($9, 36) + $10 | 0;
                         $13 = $6 + 4 | 0;
                         HEAP32[$11 + 4 >> 2] = $13;
                         $15 = $4 + 56 | 0;
                         HEAP32[$15 >> 2] = HEAP32[$15 >> 2] + 1;
                         HEAP32[$7 >> 2] = $13;
                         HEAP32[$6 + 4 >> 2] = $11;
                         HEAP32[$6 + 8 >> 2] = $7;
                         $5 = $5 + -1 | 0;
                         HEAP32[$6 + 20 >> 2] = $5;
                         HEAP32[$6 + 24 >> 2] = HEAP32[$6 + 24 >> 2] & -129;
                         $4 = $4 + -36 | 0;
                         if (($5 | 0) > ($1 | 0)) {
                          continue
                         }
                         break;
                        };
                       }
                       HEAP32[$10 + 16 >> 2] = $12;
                       if (($1 | 0) == 31) {
                        break label$45
                       }
                       $17 = 2;
                       break label$43;
                      }
                      label$53 : {
                       switch ($17 | 0) {
                       case 0:
                        $4 = $82;
                        $6 = $31;
                        $5 = $1;
                        while (1) {
                         label$56 : {
                          $9 = $6 + 24 | 0;
                          $7 = HEAP32[$9 >> 2];
                          if (($7 | 0) != ($9 | 0)) {
                           $10 = $7 + -4 | 0;
                           if ($10) {
                            break label$56
                           }
                          }
                          $4 = $4 + 36 | 0;
                          $6 = $6 + 36 | 0;
                          $5 = $5 + 1 | 0;
                          if ($5 >>> 0 < 11) {
                           continue
                          }
                          break label$37;
                         }
                         break;
                        };
                        $7 = HEAP32[$10 + 4 >> 2];
                        $9 = HEAP32[$10 + 8 >> 2];
                        HEAP32[$7 + 4 >> 2] = $9;
                        HEAP32[$9 >> 2] = $7;
                        HEAP32[$10 + 20 >> 2] = 0;
                        HEAP32[$10 + 24 >> 2] = HEAP32[$10 + 24 >> 2] | 128;
                        HEAP32[$10 + 4 >> 2] = 256;
                        HEAP32[$10 + 8 >> 2] = 512;
                        $6 = $6 + 32 | 0;
                        HEAP32[$6 >> 2] = HEAP32[$6 >> 2] + -1;
                        if (($5 | 0) > ($1 | 0)) {
                         $6 = 1 << $5;
                         while (1) {
                          $9 = HEAP32[$4 >> 2];
                          $6 = $6 >>> 1 | 0;
                          $7 = Math_imul($6, 36) + $10 | 0;
                          $11 = $7 + 4 | 0;
                          HEAP32[$9 + 4 >> 2] = $11;
                          $13 = $4 + 8 | 0;
                          HEAP32[$13 >> 2] = HEAP32[$13 >> 2] + 1;
                          HEAP32[$4 >> 2] = $11;
                          HEAP32[$7 + 4 >> 2] = $9;
                          HEAP32[$7 + 8 >> 2] = $4;
                          $5 = $5 + -1 | 0;
                          HEAP32[$7 + 20 >> 2] = $5;
                          HEAP32[$7 + 24 >> 2] = HEAP32[$7 + 24 >> 2] & -129;
                          $4 = $4 + -36 | 0;
                          if (($5 | 0) > ($1 | 0)) {
                           continue
                          }
                          break;
                         };
                        }
                        HEAP32[$10 + 16 >> 2] = 3;
                        if (($1 | 0) != 31) {
                         break label$41
                        }
                        break label$45;
                       case 1:
                        label$61 : {
                         $6 = $46;
                         $7 = Math_imul($6, 36) + $8 | 0;
                         label$62 : {
                          if (!HEAP32[$7 + 92 >> 2]) {
                           break label$62
                          }
                          $4 = HEAP32[$83 >> 2];
                          $5 = $81;
                          while (1) {
                           $9 = ($7 + ($4 << 3) | 0) + 60 | 0;
                           if (HEAP32[$9 >> 2] == ($9 | 0)) {
                            $4 = HEAP32[$5 >> 2];
                            $5 = $5 + 4 | 0;
                            if (($4 | 0) != 4) {
                             continue
                            }
                            break label$62;
                           }
                           break;
                          };
                          if (($4 | 0) != -1) {
                           break label$61
                          }
                         }
                         $46 = $6 + -1 | 0;
                         if (($6 | 0) > ($1 | 0)) {
                          break label$40
                         }
                         break label$4;
                        }
                        $11 = ($29 | 0) != 0;
                        $9 = $11 | (($78 | 0) == 2 | $6 >>> 0 > 4);
                        $5 = $7 + 60 | 0;
                        label$65 : {
                         if (($6 | 0) <= ($1 | 0)) {
                          break label$65
                         }
                         if (($12 | 0) != 1) {
                          break label$65
                         }
                         if ($9) {
                          break label$65
                         }
                         $5 = $31;
                         $6 = $1;
                         label$66 : {
                          label$67 : {
                           label$68 : {
                            while (1) {
                             if (HEAP32[$5 + 32 >> 2]) {
                              $4 = $5 + 16 | 0;
                              if (HEAP32[$4 >> 2] != ($4 | 0)) {
                               break label$68
                              }
                              if (HEAP32[$5 >> 2] != ($5 | 0)) {
                               break label$67
                              }
                             }
                             $5 = $5 + 36 | 0;
                             $6 = $6 + 1 | 0;
                             if (($6 | 0) < 11) {
                              continue
                             }
                             break;
                            };
                            $5 = $5 + -36 | 0;
                            $4 = -1;
                            $9 = 0;
                            break label$65;
                           }
                           $4 = 2;
                           break label$66;
                          }
                          $4 = 0;
                         }
                         $9 = $11 | $6 >>> 0 > 4;
                        }
                        $7 = HEAP32[($4 << 3) + $5 >> 2];
                        $11 = HEAP32[$7 + 16 >> 2];
                        $5 = $7 + -4 | 0;
                        $6 = HEAP32[$5 >> 2];
                        $4 = Math_imul($6 >>> 31 | 0, 516);
                        $15 = HEAP32[$4 + 141304 >> 2];
                        $13 = HEAP32[20646];
                        $4 = (($5 - $13 | 0) / 36 | 0) - (HEAP32[$4 + 141308 >> 2] & -1024) | 0;
                        $15 = HEAP32[$15 + ($4 >>> 11 & 2097148) >> 2] >>> 29 - ($4 >>> 8 & 28) & 7;
                        if (($15 | 0) == 3) {
                         break label$36
                        }
                        if ($11 >>> 0 >= 10) {
                         $4 = $7 + 36860 | 0;
                         $5 = 1 - (1 << $11 + -10) | 0;
                         while (1) {
                          $9 = Math_imul($6 >>> 31 | 0, 516);
                          $6 = ((($4 + -36864 | 0) - $13 | 0) / 36 | 0) - (HEAP32[$9 + 141308 >> 2] & -1024) | 0;
                          $9 = HEAP32[$9 + 141304 >> 2] + ($6 >>> 11 & 2097148) | 0;
                          $6 = 29 - ($6 >>> 8 & 28) | 0;
                          HEAP32[$9 >> 2] = HEAP32[$9 >> 2] & (7 << $6 ^ -1) | (($12 | 0) < 3 ? ($29 ? 0 : $12) : $12) << $6;
                          if (!$5) {
                           break label$36
                          }
                          $5 = $5 + 1 | 0;
                          $29 = HEAP32[20703];
                          $6 = HEAP32[$4 >> 2];
                          $4 = $4 + 36864 | 0;
                          continue;
                         };
                        }
                        if (!$9) {
                         break label$36
                        }
                        $4 = move_freepages_block($8, $5, $12, $16 + 12 | 0);
                        label$73 : {
                         if (($12 | 0) == 1) {
                          $6 = HEAP32[$16 + 12 >> 2];
                          if ($4) {
                           break label$73
                          }
                          break label$36;
                         }
                         $6 = ($15 | 0) == 1 ? (1024 - $4 | 0) - HEAP32[$16 + 12 >> 2] | 0 : 0;
                         if (!$4) {
                          break label$36
                         }
                        }
                        $7 = HEAP32[20703];
                        if ($7 ? 0 : ($4 + $6 | 0) <= 511) {
                         break label$38
                        }
                        $4 = Math_imul(HEAP32[$5 >> 2] >>> 31 | 0, 516);
                        $5 = (($5 - HEAP32[20646] | 0) / 36 | 0) - (HEAP32[$4 + 141308 >> 2] & -1024) | 0;
                        $4 = HEAP32[$4 + 141304 >> 2] + ($5 >>> 11 & 2097148) | 0;
                        $5 = 29 - ($5 >>> 8 & 28) | 0;
                        HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & (7 << $5 ^ -1) | (($12 | 0) < 3 ? ($7 ? 0 : $12) : $12) << $5;
                        if ($1 >>> 0 <= 10) {
                         break label$42
                        }
                        break label$39;
                       case 2:
                        break label$53;
                       default:
                        break label$44;
                       };
                      }
                      $5 = 0;
                      $4 = $10;
                      while (1) {
                       $7 = $4 + 24 | 0;
                       if (HEAP32[$7 >> 2] != -1) {
                        break label$35
                       }
                       $6 = HEAP32[$4 >> 2] & 4194303;
                       $9 = HEAP32[$4 + 12 >> 2];
                       if ($6 | ($9 | HEAP32[$4 + 28 >> 2])) {
                        break label$34
                       }
                       $4 = $4 + 36 | 0;
                       $5 = $5 + 1 | 0;
                       if (($5 | 0) < ($24 | 0)) {
                        continue
                       }
                       break;
                      };
                     }
                     $5 = $8 + 468 | 0;
                     HEAP32[$5 >> 2] = HEAP32[$5 >> 2] + $28;
                     HEAP32[20656] = HEAP32[20656] + $28;
                     if (!$10) {
                      break label$4
                     }
                     break label$6;
                    }
                    $5 = HEAP32[$25 >> 2];
                    label$78 : {
                     if (($25 | 0) != ($5 | 0)) {
                      $7 = $5 + -4 | 0;
                      $6 = 0;
                      if ($7) {
                       break label$78
                      }
                     }
                     while (1) {
                      label$81 : {
                       label$82 : {
                        label$83 : {
                         label$84 : {
                          label$85 : {
                           label$86 : {
                            label$87 : {
                             label$88 : {
                              label$89 : {
                               label$90 : {
                                label$91 : {
                                 label$92 : {
                                  label$93 : {
                                   $5 = HEAP32[$57 >> 2];
                                   if (($57 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$93
                                    }
                                   }
                                   $5 = HEAP32[$56 >> 2];
                                   if (($56 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$92
                                    }
                                   }
                                   $5 = HEAP32[$55 >> 2];
                                   if (($55 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$91
                                    }
                                   }
                                   $5 = HEAP32[$54 >> 2];
                                   if (($54 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$90
                                    }
                                   }
                                   $5 = HEAP32[$53 >> 2];
                                   if (($53 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$89
                                    }
                                   }
                                   $5 = HEAP32[$52 >> 2];
                                   if (($52 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$88
                                    }
                                   }
                                   $5 = HEAP32[$51 >> 2];
                                   if (($51 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$87
                                    }
                                   }
                                   $5 = HEAP32[$50 >> 2];
                                   if (($50 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$86
                                    }
                                   }
                                   $5 = HEAP32[$49 >> 2];
                                   if (($49 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$85
                                    }
                                   }
                                   $5 = HEAP32[$48 >> 2];
                                   if (($48 | 0) != ($5 | 0)) {
                                    $7 = $5 + -4 | 0;
                                    if ($7) {
                                     break label$84
                                    }
                                   }
                                   $7 = HEAP32[20703];
                                   $4 = 10;
                                   while (1) {
                                    label$104 : {
                                     $6 = $4;
                                     $9 = Math_imul($4, 36) + $8 | 0;
                                     label$106 : {
                                      if (!HEAP32[$9 + 92 >> 2]) {
                                       break label$106
                                      }
                                      $4 = HEAP32[$87 >> 2];
                                      $5 = $86;
                                      while (1) {
                                       $11 = ($9 + ($4 << 3) | 0) + 60 | 0;
                                       if (HEAP32[$11 >> 2] == ($11 | 0)) {
                                        $4 = HEAP32[$5 >> 2];
                                        $5 = $5 + 4 | 0;
                                        if (($4 | 0) != 4) {
                                         continue
                                        }
                                        break label$106;
                                       }
                                       break;
                                      };
                                      if (($4 | 0) != -1) {
                                       break label$104
                                      }
                                     }
                                     $4 = $6 + -1 | 0;
                                     if ($6) {
                                      continue
                                     }
                                     break label$8;
                                    }
                                    break;
                                   };
                                   $13 = ($7 | 0) != 0;
                                   $11 = $13 | (($84 | 0) == 2 | $6 >>> 0 > 4);
                                   $5 = $9 + 60 | 0;
                                   if (($6 | 0) < 1) {
                                    break label$81
                                   }
                                   if (($12 | 0) != 1) {
                                    break label$81
                                   }
                                   if ($11) {
                                    break label$81
                                   }
                                   if (HEAP32[$8 + 92 >> 2]) {
                                    if (($68 | 0) != HEAP32[$68 >> 2]) {
                                     $5 = 0;
                                     $4 = 2;
                                     break label$82;
                                    }
                                    $5 = 0;
                                    $4 = 0;
                                    if (HEAP32[$41 >> 2] != ($41 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 128 >> 2]) {
                                    $5 = 1;
                                    $4 = 2;
                                    if (HEAP32[$67 >> 2] != ($67 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$40 >> 2] != ($40 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 164 >> 2]) {
                                    $5 = 2;
                                    if (HEAP32[$66 >> 2] != ($66 | 0)) {
                                     break label$83
                                    }
                                    $4 = 0;
                                    if (HEAP32[$39 >> 2] != ($39 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 200 >> 2]) {
                                    $5 = 3;
                                    $4 = 2;
                                    if (HEAP32[$65 >> 2] != ($65 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$38 >> 2] != ($38 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 236 >> 2]) {
                                    $5 = 4;
                                    $4 = 2;
                                    if (HEAP32[$64 >> 2] != ($64 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$37 >> 2] != ($37 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 272 >> 2]) {
                                    $5 = 5;
                                    $4 = 2;
                                    if (HEAP32[$63 >> 2] != ($63 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$36 >> 2] != ($36 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 308 >> 2]) {
                                    $5 = 6;
                                    $4 = 2;
                                    if (HEAP32[$62 >> 2] != ($62 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$35 >> 2] != ($35 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 344 >> 2]) {
                                    $5 = 7;
                                    $4 = 2;
                                    if (HEAP32[$61 >> 2] != ($61 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$34 >> 2] != ($34 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 380 >> 2]) {
                                    $5 = 8;
                                    $4 = 2;
                                    if (HEAP32[$60 >> 2] != ($60 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$33 >> 2] != ($33 | 0)) {
                                     break label$82
                                    }
                                   }
                                   if (HEAP32[$8 + 416 >> 2]) {
                                    $5 = 9;
                                    $4 = 2;
                                    if (HEAP32[$59 >> 2] != ($59 | 0)) {
                                     break label$82
                                    }
                                    $4 = 0;
                                    if (HEAP32[$32 >> 2] != ($32 | 0)) {
                                     break label$82
                                    }
                                   }
                                   $4 = -1;
                                   if (!HEAP32[$8 + 452 >> 2]) {
                                    $11 = 0;
                                    $5 = $23;
                                    break label$81;
                                   }
                                   $5 = 10;
                                   if (HEAP32[$58 >> 2] != ($58 | 0)) {
                                    break label$83
                                   }
                                   $11 = 0;
                                   if (($23 | 0) == HEAP32[$23 >> 2]) {
                                    $5 = $23;
                                    break label$81;
                                   }
                                   $4 = 0;
                                   break label$82;
                                  }
                                  $6 = 1;
                                  break label$78;
                                 }
                                 $6 = 2;
                                 break label$78;
                                }
                                $6 = 3;
                                break label$78;
                               }
                               $6 = 4;
                               break label$78;
                              }
                              $6 = 5;
                              break label$78;
                             }
                             $6 = 6;
                             break label$78;
                            }
                            $6 = 7;
                            break label$78;
                           }
                           $6 = 8;
                           break label$78;
                          }
                          $6 = 9;
                          break label$78;
                         }
                         $6 = 10;
                         break label$78;
                        }
                        $4 = 2;
                       }
                       $11 = $13 | $5 >>> 0 > 4;
                       $5 = (Math_imul($5, 36) + $8 | 0) + 60 | 0;
                      }
                      $9 = HEAP32[($4 << 3) + $5 >> 2];
                      $13 = HEAP32[$9 + 16 >> 2];
                      label$122 : {
                       label$123 : {
                        $5 = $9 + -4 | 0;
                        $6 = HEAP32[$5 >> 2];
                        $4 = Math_imul($6 >>> 31 | 0, 516);
                        $18 = HEAP32[$4 + 141304 >> 2];
                        $15 = HEAP32[20646];
                        $4 = (($5 - $15 | 0) / 36 | 0) - (HEAP32[$4 + 141308 >> 2] & -1024) | 0;
                        $18 = HEAP32[$18 + ($4 >>> 11 & 2097148) >> 2] >>> 29 - ($4 >>> 8 & 28) & 7;
                        if (($18 | 0) == 3) {
                         break label$123
                        }
                        if ($13 >>> 0 >= 10) {
                         $4 = $9 + 36860 | 0;
                         $5 = 1 - (1 << $13 + -10) | 0;
                         while (1) {
                          $11 = Math_imul($6 >>> 31 | 0, 516);
                          $6 = ((($4 + -36864 | 0) - $15 | 0) / 36 | 0) - (HEAP32[$11 + 141308 >> 2] & -1024) | 0;
                          $11 = HEAP32[$11 + 141304 >> 2] + ($6 >>> 11 & 2097148) | 0;
                          $6 = 29 - ($6 >>> 8 & 28) | 0;
                          HEAP32[$11 >> 2] = HEAP32[$11 >> 2] & (7 << $6 ^ -1) | (($12 | 0) < 3 ? ($7 ? 0 : $12) : $12) << $6;
                          if (!$5) {
                           break label$123
                          }
                          $5 = $5 + 1 | 0;
                          $7 = HEAP32[20703];
                          $6 = HEAP32[$4 >> 2];
                          $4 = $4 + 36864 | 0;
                          continue;
                         };
                        }
                        if (!$11) {
                         break label$123
                        }
                        $4 = move_freepages_block($8, $5, $12, $16 + 8 | 0);
                        label$126 : {
                         if (($12 | 0) != 1) {
                          $6 = ($18 | 0) == 1 ? (1024 - $4 | 0) - HEAP32[$16 + 8 >> 2] | 0 : 0;
                          break label$126;
                         }
                         $6 = HEAP32[$16 + 8 >> 2];
                        }
                        if (!$4) {
                         break label$123
                        }
                        $7 = HEAP32[20703];
                        if ($7 ? 0 : ($4 + $6 | 0) <= 511) {
                         break label$122
                        }
                        $4 = Math_imul(HEAP32[$5 >> 2] >>> 31 | 0, 516);
                        $5 = (($5 - HEAP32[20646] | 0) / 36 | 0) - (HEAP32[$4 + 141308 >> 2] & -1024) | 0;
                        $4 = HEAP32[$4 + 141304 >> 2] + ($5 >>> 11 & 2097148) | 0;
                        $5 = 29 - ($5 >>> 8 & 28) | 0;
                        HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & (7 << $5 ^ -1) | (($12 | 0) < 3 ? ($7 ? 0 : $12) : $12) << $5;
                        break label$122;
                       }
                       $5 = HEAP32[$9 >> 2];
                       $4 = HEAP32[$9 + 4 >> 2];
                       HEAP32[$5 + 4 >> 2] = $4;
                       HEAP32[$4 >> 2] = $5;
                       $5 = ((Math_imul($13, 36) + $8 | 0) + $14 | 0) + 60 | 0;
                       $4 = HEAP32[$5 >> 2];
                       HEAP32[$4 + 4 >> 2] = $9;
                       HEAP32[$5 >> 2] = $9;
                       HEAP32[$9 >> 2] = $4;
                       HEAP32[$9 + 4 >> 2] = $5;
                      }
                      $5 = HEAP32[$25 >> 2];
                      if (($5 | 0) == ($25 | 0)) {
                       continue
                      }
                      $7 = $5 + -4 | 0;
                      if (!$7) {
                       continue
                      }
                      break;
                     };
                     $6 = 0;
                    }
                    $5 = HEAP32[$7 + 4 >> 2];
                    $4 = HEAP32[$7 + 8 >> 2];
                    HEAP32[$5 + 4 >> 2] = $4;
                    HEAP32[$4 >> 2] = $5;
                    HEAP32[$7 + 20 >> 2] = 0;
                    HEAP32[$7 + 24 >> 2] = HEAP32[$7 + 24 >> 2] | 128;
                    HEAP32[$7 + 4 >> 2] = 256;
                    HEAP32[$7 + 8 >> 2] = 512;
                    $5 = Math_imul($6, 36) + $8 | 0;
                    $4 = $5 + 92 | 0;
                    HEAP32[$4 >> 2] = HEAP32[$4 >> 2] + -1;
                    if ($6) {
                     $9 = 1 << $6;
                     while (1) {
                      $11 = $5 + $85 | 0;
                      $13 = HEAP32[$11 >> 2];
                      $9 = $9 >>> 1 | 0;
                      $4 = Math_imul($9, 36) + $7 | 0;
                      $15 = $4 + 4 | 0;
                      HEAP32[$13 + 4 >> 2] = $15;
                      $18 = $5 + 56 | 0;
                      HEAP32[$18 >> 2] = HEAP32[$18 >> 2] + 1;
                      HEAP32[$11 >> 2] = $15;
                      HEAP32[$4 + 4 >> 2] = $13;
                      HEAP32[$4 + 8 >> 2] = $11;
                      $6 = $6 + -1 | 0;
                      HEAP32[$4 + 20 >> 2] = $6;
                      HEAP32[$4 + 24 >> 2] = HEAP32[$4 + 24 >> 2] & -129;
                      $5 = $5 + -36 | 0;
                      if (($6 | 0) > 0) {
                       continue
                      }
                      break;
                     };
                    }
                    HEAP32[$7 + 16 >> 2] = $12;
                    label$132 : {
                     label$133 : {
                      label$134 : {
                       if (HEAP32[$7 + 24 >> 2] != -1) {
                        $4 = HEAP32[$7 >> 2] & 4194303;
                        $5 = HEAP32[$7 + 12 >> 2];
                        break label$134;
                       }
                       $4 = HEAP32[$7 >> 2] & 4194303;
                       $5 = HEAP32[$7 + 12 >> 2];
                       if (!($4 | ($5 | HEAP32[$7 + 28 >> 2]))) {
                        break label$133
                       }
                      }
                      bad_page($7, $4 ? 28033 : HEAP32[$7 + 28 >> 2] ? 28018 : $5 ? 27794 : HEAP32[$7 + 24 >> 2] == -1 ? 0 : 27777, $4 ? 4194303 : 0);
                      break label$132;
                     }
                     $5 = HEAP32[$69 >> 2];
                     $4 = $7 + 4 | 0;
                     HEAP32[$69 >> 2] = $4;
                     HEAP32[$7 + 4 >> 2] = $22;
                     HEAP32[$5 >> 2] = $4;
                     HEAP32[$7 + 8 >> 2] = $5;
                     $26 = $26 + 1 | 0;
                    }
                    $21 = $21 + 1 | 0;
                    if ($21 >>> 0 >= $47 >>> 0) {
                     break label$8
                    }
                    $17 = 3;
                    $4 = 1;
                    continue;
                   }
                   $4 = 1;
                   continue;
                  }
                  $4 = 0;
                  continue;
                 }
                 $17 = 2;
                 $4 = 1;
                 continue;
                }
                $17 = 1;
                $4 = 1;
                continue;
               }
               $4 = 2;
               continue label$3;
              }
              $4 = 1;
              continue label$3;
             }
             $4 = 1;
             continue label$3;
            }
            $5 = HEAP32[$7 >> 2];
            $4 = HEAP32[$7 + 4 >> 2];
            HEAP32[$5 + 4 >> 2] = $4;
            HEAP32[$4 >> 2] = $5;
            $5 = ((Math_imul($11, 36) + $8 | 0) + $30 | 0) + 60 | 0;
            $4 = HEAP32[$5 >> 2];
            HEAP32[$4 + 4 >> 2] = $7;
            HEAP32[$5 >> 2] = $7;
            HEAP32[$7 >> 2] = $4;
            HEAP32[$7 + 4 >> 2] = $5;
            $4 = 1;
            continue label$3;
           }
           $6 = HEAP32[$4 >> 2] & 4194303;
           $9 = HEAP32[$4 + 12 >> 2];
          }
          bad_page($4, $6 ? 28033 : HEAP32[$4 + 28 >> 2] ? 28018 : $9 ? 27794 : HEAP32[$7 >> 2] == -1 ? 0 : 27777, $6 ? 4194303 : 0);
          if ($43) {
           $17 = 0;
           $4 = 1;
           continue;
          } else {
           $4 = 1;
           continue label$3;
          }
         };
        }
        $5 = $8 + 468 | 0;
        HEAP32[$5 >> 2] = HEAP32[$5 >> 2] - $21;
        HEAP32[20656] = HEAP32[20656] - $21;
        HEAP32[$20 >> 2] = HEAP32[$20 >> 2] + $26;
        $4 = HEAP32[$22 >> 2];
        if (($22 | 0) == ($4 | 0)) {
         break label$4
        }
       }
       $5 = HEAP32[$4 >> 2];
       $6 = HEAP32[$4 + 4 >> 2];
       HEAP32[$5 + 4 >> 2] = $6;
       HEAP32[$6 >> 2] = $5;
       HEAP32[$4 >> 2] = 256;
       HEAP32[$4 + 4 >> 2] = 512;
       HEAP32[$20 >> 2] = HEAP32[$20 >> 2] + -1;
       $10 = $4 + -4 | 0;
       if (!$10) {
        break label$4
       }
      }
      HEAP32[$10 + 28 >> 2] = 1;
      HEAP32[$10 + 20 >> 2] = 0;
      label$137 : {
       if (!($0 & 32768)) {
        break label$137
       }
       if (($1 | 0) == 31) {
        break label$137
       }
       $4 = $10 + 32 | 0;
       $5 = 0;
       while (1) {
        $3 = HEAP32[2];
        HEAP32[$3 + 880 >> 2] = HEAP32[$3 + 880 >> 2] + 1;
        memset(HEAP32[$4 >> 2], 0, 65536);
        $3 = HEAP32[2];
        HEAP32[$3 + 880 >> 2] = HEAP32[$3 + 880 >> 2] + -1;
        $4 = $4 + 36 | 0;
        $5 = $5 + 1 | 0;
        if (($5 | 0) < ($24 | 0)) {
         continue
        }
        break;
       };
      }
      label$139 : {
       if (!$1) {
        break label$139
       }
       if (!($0 & 16384)) {
        break label$139
       }
       HEAP8[$10 + 45 | 0] = $1;
       HEAP8[$10 + 44 | 0] = 1;
       HEAP32[$10 >> 2] = HEAP32[$10 >> 2] | 65536;
       if (($24 | 0) >= 2) {
        $4 = $10 + 40 | 0;
        $5 = $24 + -1 | 0;
        $0 = $10 + 1 | 0;
        while (1) {
         HEAP32[$4 + 24 >> 2] = 0;
         HEAP32[$4 + 8 >> 2] = 1024;
         HEAP32[$4 >> 2] = $0;
         $4 = $4 + 36 | 0;
         $5 = $5 + -1 | 0;
         if ($5) {
          continue
         }
         break;
        };
       }
       HEAP32[$10 + 48 >> 2] = -1;
      }
      HEAP32[$10 + 16 >> 2] = $2 << 29 >> 31;
      if (!$42) {
       break label$1
      }
      if (!$1) {
       break label$1
      }
      $0 = (HEAPU32[$8 + 40 >> 2] / 100 | 0) + 1024 | 0;
      if (HEAPU32[$8 + 12 >> 2] >= $0 >>> 0) {
       break label$1
      }
      $3 = HEAP32[$8 + 12 >> 2];
      label$142 : {
       if ($3 >>> 0 >= $0 >>> 0) {
        break label$142
       }
       $5 = HEAP32[20646];
       $0 = ($10 - $5 | 0) / 36 | 0;
       $2 = Math_imul(HEAP32[$10 >> 2] >>> 31 | 0, 516);
       $1 = $0 - (HEAP32[$2 + 141308 >> 2] & -1024) | 0;
       $2 = HEAP32[$2 + 141304 >> 2] + ($1 >>> 11 & 2097148) | 0;
       $1 = 29 - ($1 >>> 8 & 28) | 0;
       if ((HEAP32[$2 >> 2] >>> $1 & 7) == 3) {
        break label$142
       }
       HEAP32[$8 + 12 >> 2] = $3 + 1024;
       HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & (7 << $1 ^ -1) | 3 << $1;
       $1 = $0 | 1023;
       $2 = $0 & -1024;
       $3 = $5 + Math_imul($2, 36) | 0;
       $4 = $10;
       $0 = HEAP32[$8 + 36 >> 2];
       if ($0 >>> 0 <= $2 >>> 0) {
        $4 = $0 + HEAP32[$8 + 44 >> 2] >>> 0 > $2 >>> 0 ? $3 : $10
       }
       if ($0 >>> 0 > $1 >>> 0) {
        break label$142
       }
       $2 = $3 + 36828 | 0;
       if ($4 >>> 0 > $2 >>> 0) {
        break label$142
       }
       if ($0 + HEAP32[$8 + 44 >> 2] >>> 0 <= $1 >>> 0) {
        break label$142
       }
       while (1) {
        if ((HEAP32[$4 + 24 >> 2] & -268435328) == -268435456) {
         $0 = HEAP32[$4 + 20 >> 2];
         $1 = HEAP32[$4 + 4 >> 2];
         $3 = $4 + 8 | 0;
         $5 = HEAP32[$3 >> 2];
         HEAP32[$1 + 4 >> 2] = $5;
         HEAP32[$5 >> 2] = $1;
         $1 = (Math_imul($0, 36) + $8 | 0) + 84 | 0;
         $5 = HEAP32[$1 >> 2];
         $6 = $4 + 4 | 0;
         HEAP32[$5 + 4 >> 2] = $6;
         HEAP32[$4 + 4 >> 2] = $5;
         HEAP32[$1 >> 2] = $6;
         HEAP32[$3 >> 2] = $1;
         $4 = Math_imul(1 << $0, 36) + $4 | 0;
         if ($4 >>> 0 <= $2 >>> 0) {
          continue
         }
         break label$142;
        }
        $4 = $4 + 36 | 0;
        if ($4 >>> 0 <= $2 >>> 0) {
         continue
        }
        break;
       };
      }
      break label$1;
     }
     $5 = $19 + 8 | 0;
     $4 = HEAP32[$3 + 16 >> 2];
     label$146 : {
      label$147 : {
       $6 = HEAP32[$3 + 4 >> 2];
       if (!(HEAPU32[$19 + 12 >> 2] <= $4 >>> 0 ? !$6 : 0)) {
        $19 = __next_zones_zonelist($5, $4, $6);
        $8 = HEAP32[$19 >> 2];
        if ($8) {
         break label$147
        }
        break label$2;
       }
       $19 = $5;
       $8 = HEAP32[$5 >> 2];
       if ($8) {
        break label$146
       }
       break label$2;
      }
      $4 = 0;
      continue;
     }
     $4 = 0;
     continue;
    };
   }
   $10 = 0;
  }
  global$0 = $16 + 16 | 0;
  return $10;
 }
 
 function unreserve_highatomic_pageblock($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0;
  $2 = HEAP32[$0 + 16 >> 2];
  $7 = HEAP32[$0 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     $4 = HEAP32[$0 + 4 >> 2];
     if ($4) {
      break label$3
     }
     if (HEAPU32[$7 + 4 >> 2] > $2 >>> 0) {
      break label$3
     }
     $4 = HEAP32[$7 >> 2];
     if (!$4) {
      break label$1
     }
     break label$2;
    }
    $7 = __next_zones_zonelist($7, $2, $4);
    $4 = HEAP32[$7 >> 2];
    if (!$4) {
     break label$1
    }
   }
   $14 = $0 + 16 | 0;
   while (1) {
    label$4 : {
     if (!(HEAPU32[$4 + 12 >> 2] < 1025 ? !$1 : 0)) {
      $10 = 0;
      while (1) {
       $3 = (Math_imul($10, 36) + $4 | 0) + 84 | 0;
       $2 = HEAP32[$3 >> 2];
       label$9 : {
        if (($2 | 0) == ($3 | 0)) {
         break label$9
        }
        $2 = $2 + -4 | 0;
        if (!$2) {
         break label$9
        }
        $9 = HEAP32[20646];
        $5 = ($2 - $9 | 0) / 36 | 0;
        $6 = Math_imul(HEAP32[$2 >> 2] >>> 31 | 0, 516);
        $3 = $5 - (HEAP32[$6 + 141308 >> 2] & -1024) | 0;
        $6 = HEAP32[$6 + 141304 >> 2] + ($3 >>> 11 & 2097148) | 0;
        $8 = 29 - ($3 >>> 8 & 28) | 0;
        if ((HEAP32[$6 >> 2] >>> $8 & 7) == 3) {
         $11 = $4 + 12 | 0;
         $3 = HEAP32[$11 >> 2];
         HEAP32[$11 >> 2] = $3 - ($3 >>> 0 < 1024 ? $3 : 1024);
        }
        $11 = $0 + 12 | 0;
        $3 = HEAP32[$11 >> 2];
        HEAP32[$6 >> 2] = HEAP32[$6 >> 2] & (7 << $8 ^ -1) | (($3 | 0) < 3 ? (HEAP32[20703] ? 0 : $3) : $3) << $8;
        $6 = $5 | 1023;
        $5 = $5 & -1024;
        $8 = $9 + Math_imul($5, 36) | 0;
        $3 = HEAP32[$4 + 36 >> 2];
        if ($3 >>> 0 <= $5 >>> 0) {
         $2 = $3 + HEAP32[$4 + 44 >> 2] >>> 0 > $5 >>> 0 ? $8 : $2
        }
        if ($3 >>> 0 > $6 >>> 0) {
         break label$9
        }
        $8 = $8 + 36828 | 0;
        if ($2 >>> 0 > $8 >>> 0) {
         break label$9
        }
        if ($3 + HEAP32[$4 + 44 >> 2] >>> 0 <= $6 >>> 0) {
         break label$9
        }
        $11 = HEAP32[$11 >> 2];
        $3 = 0;
        while (1) {
         label$12 : {
          if ((HEAP32[$2 + 24 >> 2] & -268435328) != -268435456) {
           $2 = $2 + 36 | 0;
           if ($2 >>> 0 <= $8 >>> 0) {
            continue
           }
           break label$12;
          }
          $6 = HEAP32[$2 + 20 >> 2];
          $5 = $2 + 4 | 0;
          $9 = HEAP32[$5 >> 2];
          $13 = $2 + 8 | 0;
          $12 = HEAP32[$13 >> 2];
          HEAP32[$9 + 4 >> 2] = $12;
          HEAP32[$12 >> 2] = $9;
          $9 = ((Math_imul($6, 36) + $4 | 0) + ($11 << 3) | 0) + 60 | 0;
          $12 = HEAP32[$9 >> 2];
          HEAP32[$12 + 4 >> 2] = $5;
          HEAP32[$9 >> 2] = $5;
          HEAP32[$5 >> 2] = $12;
          HEAP32[$13 >> 2] = $9;
          $5 = 1 << $6;
          $3 = $5 + $3 | 0;
          $2 = Math_imul($5, 36) + $2 | 0;
          if ($2 >>> 0 <= $8 >>> 0) {
           continue
          }
         }
         break;
        };
        if ($3) {
         break label$4
        }
       }
       $10 = $10 + 1 | 0;
       if ($10 >>> 0 < 11) {
        continue
       }
       break;
      };
     }
     $2 = $7 + 8 | 0;
     $4 = HEAP32[$14 >> 2];
     label$15 : {
      $10 = HEAP32[$0 + 4 >> 2];
      if ($10) {
       break label$15
      }
      if (HEAPU32[$7 + 12 >> 2] > $4 >>> 0) {
       break label$15
      }
      $7 = $2;
      $4 = HEAP32[$2 >> 2];
      if ($4) {
       continue
      }
      break label$1;
     }
     $7 = __next_zones_zonelist($2, $4, $10);
     $4 = HEAP32[$7 >> 2];
     if ($4) {
      continue
     }
     break label$1;
    }
    break;
   };
   return 1;
  }
  return 0;
 }
 
 function free_pages($0, $1) {
  var $2 = 0;
  label$1 : {
   if ($0) {
    $0 = HEAP32[20646] + Math_imul($0 >>> 16 | 0, 36) | 0;
    $2 = HEAP32[$0 + 28 >> 2] + -1 | 0;
    HEAP32[$0 + 28 >> 2] = $2;
    if (!$2) {
     break label$1
    }
   }
   return;
  }
  if ($1) {
   __free_pages_ok($0, $1);
   return;
  }
  free_unref_page($0);
 }
 
 function show_free_areas($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0, $35 = 0, $36 = 0, $37 = 0, $38 = 0, $39 = 0;
  $2 = global$0 - 416 | 0;
  global$0 = $2;
  $3 = 141272;
  if ($3) {
   $5 = $1 ? $1 : 16876;
   $6 = $0 & 1;
   while (1) {
    label$3 : {
     if (!HEAP32[$3 + 48 >> 2]) {
      break label$3
     }
     if (HEAP32[$5 >> 2] & 1 ? 0 : $6) {
      break label$3
     }
     $4 = HEAP32[HEAP32[$3 + 28 >> 2] >> 2] + $4 | 0;
    }
    $3 = HEAP32[$3 + 24 >> 2] + 516 >>> 0 > $3 >>> 0 ? $3 + 516 | 0 : 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  HEAP32[$2 + 324 >> 2] = $4;
  $3 = HEAP32[20669];
  $4 = HEAP32[20668];
  $5 = HEAP32[20675];
  $6 = HEAP32[20671];
  HEAP32[$2 + 272 >> 2] = HEAP32[20670];
  HEAP32[$2 + 276 >> 2] = HEAP32[20676];
  HEAP32[$2 + 280 >> 2] = HEAP32[20672];
  HEAP32[$2 + 284 >> 2] = HEAP32[20685];
  HEAP32[$2 + 288 >> 2] = HEAP32[20686];
  HEAP32[$2 + 292 >> 2] = HEAP32[20692];
  HEAP32[$2 + 296 >> 2] = HEAP32[20673];
  HEAP32[$2 + 300 >> 2] = HEAP32[20674];
  HEAP32[$2 + 304 >> 2] = HEAP32[20683];
  HEAP32[$2 + 308 >> 2] = HEAP32[20688];
  HEAP32[$2 + 312 >> 2] = HEAP32[20664];
  HEAP32[$2 + 316 >> 2] = HEAP32[20666];
  HEAP32[$2 + 320 >> 2] = HEAP32[20656];
  HEAP32[$2 + 328 >> 2] = HEAP32[20667];
  HEAP32[$2 + 256 >> 2] = $3;
  HEAP32[$2 + 260 >> 2] = $4;
  HEAP32[$2 + 264 >> 2] = $5;
  HEAP32[$2 + 268 >> 2] = $6;
  printk(26614, $2 + 256 | 0);
  $3 = 141272;
  label$5 : {
   if (!$3) {
    break label$5
   }
   $5 = $1 ? $1 : 16876;
   $6 = $0 & 1;
   $9 = $2 + 248 | 0;
   $8 = $2 + 208 | 0;
   $10 = $2 + 212 | 0;
   $12 = $2 + 216 | 0;
   $13 = $2 + 220 | 0;
   $11 = $2 + 224 | 0;
   $7 = $2 + 228 | 0;
   $14 = $2 + 232 | 0;
   $15 = $2 + 236 | 0;
   $16 = $2 + 240 | 0;
   $17 = $2 + 244 | 0;
   while (1) {
    $4 = HEAP32[$3 + 1072 >> 2];
    label$7 : {
     if (!$6) {
      break label$7
     }
     if (HEAP32[$5 + (($4 | 0) / 32 << 2) >> 2] & 1 << ($4 & 31)) {
      break label$7
     }
     $3 = 0;
     if ($3) {
      continue
     }
     break label$5;
    }
    HEAP32[$9 >> 2] = HEAP32[$3 + 1104 >> 2] > 15 ? 27158 : 27162;
    $3 = HEAP32[20669];
    $18 = HEAP32[20668];
    $19 = HEAP32[20671];
    HEAP32[$8 >> 2] = HEAP32[20670] << 6;
    HEAP32[$10 >> 2] = HEAP32[20672] << 6;
    HEAP32[$12 >> 2] = HEAP32[20675] << 6;
    HEAP32[$13 >> 2] = HEAP32[20676] << 6;
    HEAP32[$11 >> 2] = HEAP32[20683] << 6;
    HEAP32[$7 >> 2] = HEAP32[20685] << 6;
    HEAP32[$14 >> 2] = HEAP32[20686] << 6;
    HEAP32[$15 >> 2] = HEAP32[20688] << 6;
    HEAP32[$16 >> 2] = HEAP32[20687] << 6;
    HEAP32[$17 >> 2] = HEAP32[20692] << 6;
    HEAP32[$2 + 192 >> 2] = $4;
    HEAP32[$2 + 196 >> 2] = $3 << 6;
    HEAP32[$2 + 200 >> 2] = $18 << 6;
    HEAP32[$2 + 204 >> 2] = $19 << 6;
    printk(26903, $2 + 192 | 0);
    $3 = 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  $3 = 141272;
  label$8 : {
   if (!$3) {
    break label$8
   }
   $4 = $1 ? $1 : 16876;
   $5 = $0 & 1;
   $6 = $2 + 184 | 0;
   $9 = $2 + 180 | 0;
   $8 = $2 + 176 | 0;
   $10 = $2 + 172 | 0;
   $12 = $2 + 168 | 0;
   $13 = $2 + 164 | 0;
   $11 = $2 + 160 | 0;
   $7 = $2 + 156 | 0;
   $14 = $2 + 152 | 0;
   $15 = $2 + 148 | 0;
   $16 = $2 + 144 | 0;
   $17 = $2 + 140 | 0;
   $18 = $2 + 136 | 0;
   $19 = $2 + 128 | 0;
   $21 = $2 + 188 | 0;
   while (1) {
    label$10 : {
     $20 = HEAP32[$3 + 48 >> 2];
     if ($20) {
      if (!$5) {
       break label$10
      }
      if (HEAP32[$4 >> 2] & 1) {
       break label$10
      }
     }
     $3 = HEAP32[$3 + 24 >> 2] + 516 >>> 0 > $3 >>> 0 ? $3 + 516 | 0 : 0;
     if ($3) {
      continue
     }
     break label$8;
    }
    $22 = HEAP32[HEAP32[$3 + 28 >> 2] >> 2];
    $23 = HEAP32[$3 + 468 >> 2];
    $24 = HEAP32[$3 + 476 >> 2];
    $25 = HEAP32[$3 + 472 >> 2];
    $26 = HEAP32[$3 + 484 >> 2];
    $27 = HEAP32[$3 + 480 >> 2];
    $28 = HEAP32[$3 + 488 >> 2];
    $29 = HEAP32[$3 + 492 >> 2];
    $30 = HEAP32[$3 + 496 >> 2];
    $31 = HEAP32[$3 + 504 >> 2];
    $32 = HEAP32[$3 + 500 >> 2];
    $33 = HEAP32[$3 + 508 >> 2];
    $34 = HEAP32[$3 + 52 >> 2];
    $35 = HEAP32[$3 >> 2];
    $36 = HEAP32[$3 + 4 >> 2];
    $37 = HEAP32[$3 + 8 >> 2];
    $38 = HEAP32[$3 + 40 >> 2];
    $39 = HEAP32[$3 + 512 >> 2];
    HEAP32[$6 >> 2] = HEAP32[HEAP32[$3 + 28 >> 2] >> 2] << 6;
    HEAP32[$9 >> 2] = $22 << 6;
    HEAP32[$8 >> 2] = $33 << 6;
    HEAP32[$10 >> 2] = $32 << 6;
    HEAP32[$12 >> 2] = $31;
    HEAP32[$13 >> 2] = $30 << 6;
    HEAP32[$11 >> 2] = $38 << 6;
    HEAP32[$7 >> 2] = $20 << 6;
    HEAP32[$14 >> 2] = $29 << 6;
    HEAP32[$15 >> 2] = $28 << 6;
    HEAP32[$16 >> 2] = $27 << 6;
    HEAP32[$17 >> 2] = $26 << 6;
    HEAP32[$18 >> 2] = $25 << 6;
    HEAP32[$2 + 132 >> 2] = $24 << 6;
    HEAP32[$19 >> 2] = $37 << 6;
    HEAP32[$21 >> 2] = $39 << 6;
    HEAP32[$2 + 124 >> 2] = $36 << 6;
    HEAP32[$2 + 120 >> 2] = $35 << 6;
    HEAP32[$2 + 116 >> 2] = $23 << 6;
    HEAP32[$2 + 112 >> 2] = $34;
    printk(27165, $2 + 112 | 0);
    printk(27462, 0);
    HEAP32[$2 + 96 >> 2] = HEAP32[$3 + 16 >> 2];
    printk(27480, $2 + 96 | 0);
    HEAP32[$2 + 80 >> 2] = HEAP32[$3 + 20 >> 2];
    printk(27480, $2 + 80 | 0);
    printk(27487, 0);
    $3 = HEAP32[$3 + 24 >> 2] + 516 >>> 0 > $3 >>> 0 ? $3 + 516 | 0 : 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  $5 = 141272;
  label$12 : {
   if (!$5) {
    break label$12
   }
   $12 = $1 ? $1 : 16876;
   $13 = $0 & 1;
   $1 = $2 + 412 | 0;
   while (1) {
    label$14 : {
     if (HEAP32[$5 + 48 >> 2]) {
      if (!$13) {
       break label$14
      }
      if (HEAP32[$12 >> 2] & 1) {
       break label$14
      }
     }
     $5 = HEAP32[$5 + 24 >> 2] + 516 >>> 0 > $5 >>> 0 ? $5 + 516 | 0 : 0;
     if ($5) {
      continue
     }
     break label$12;
    }
    HEAP32[$2 + 64 >> 2] = HEAP32[$5 + 52 >> 2];
    printk(27491, $2 - -64 | 0);
    $9 = $2 + 352 | 0;
    $8 = 0;
    $10 = 0;
    $0 = 0;
    while (1) {
     $3 = $5 + $8 | 0;
     $11 = HEAP32[$3 + 92 >> 2];
     HEAP32[$9 >> 2] = $11;
     $6 = ($2 + 341 | 0) + $0 | 0;
     HEAP8[$6 | 0] = 0;
     $4 = 0;
     $7 = $3 + 60 | 0;
     if (HEAP32[$7 >> 2] != ($7 | 0)) {
      HEAP8[$6 | 0] = 1;
      $4 = 1;
     }
     $7 = $3 + 68 | 0;
     if (HEAP32[$7 >> 2] != ($7 | 0)) {
      $4 = $4 | 2;
      HEAP8[$6 | 0] = $4;
     }
     $7 = $3 + 76 | 0;
     if (HEAP32[$7 >> 2] != ($7 | 0)) {
      $4 = $4 | 4;
      HEAP8[$6 | 0] = $4;
     }
     $3 = $3 + 84 | 0;
     if (HEAP32[$3 >> 2] != ($3 | 0)) {
      HEAP8[$6 | 0] = $4 | 8
     }
     $3 = $11 << $0;
     $9 = $9 + 4 | 0;
     $0 = $0 + 1 | 0;
     $10 = $3 + $10 | 0;
     $8 = $8 + 36 | 0;
     if (($8 | 0) != 396) {
      continue
     }
     break;
    };
    $3 = 0;
    $4 = $2 + 352 | 0;
    while (1) {
     $0 = HEAP32[$4 >> 2];
     HEAP32[$2 + 52 >> 2] = 64 << $3;
     HEAP32[$2 + 48 >> 2] = $0;
     printk(27498, $2 + 48 | 0);
     if ($0) {
      $0 = $2 + 411 | 0;
      $6 = HEAPU8[($2 + 341 | 0) + $3 | 0];
      if ($6 & 1) {
       HEAP8[$2 + 411 | 0] = 85;
       $0 = $1;
      }
      if ($6 & 2) {
       HEAP8[$0 | 0] = 77;
       $0 = $0 + 1 | 0;
      }
      if ($6 & 4) {
       HEAP8[$0 | 0] = 69;
       $0 = $0 + 1 | 0;
      }
      if ($6 & 8) {
       HEAP8[$0 | 0] = 72;
       $0 = $0 + 1 | 0;
      }
      HEAP8[$0 | 0] = 0;
      HEAP32[$2 + 32 >> 2] = $2 + 411;
      printk(28101, $2 + 32 | 0);
     }
     $4 = $4 + 4 | 0;
     $3 = $3 + 1 | 0;
     if (($3 | 0) != 11) {
      continue
     }
     break;
    };
    HEAP32[$2 + 16 >> 2] = $10 << 6;
    printk(27511, $2 + 16 | 0);
    $5 = HEAP32[$5 + 24 >> 2] + 516 >>> 0 > $5 >>> 0 ? $5 + 516 | 0 : 0;
    if ($5) {
     continue
    }
    break;
   };
  }
  HEAP32[$2 >> 2] = HEAP32[20684];
  printk(27522, $2);
  global$0 = $2 + 416 | 0;
 }
 
 function build_all_zonelists() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  label$1 : {
   if (HEAP32[16384]) {
    build_zonelists();
    break label$1;
   }
   build_all_zonelists_init();
  }
  $1 = 142304;
  label$5 : {
   label$6 : {
    label$7 : {
     if (HEAPU32[35577] < 2) {
      $0 = HEAP32[35576];
      if (!$0) {
       break label$7
      }
      break label$6;
     }
     $1 = __next_zones_zonelist(142304, 1, 0);
     $0 = HEAP32[$1 >> 2];
     if ($0) {
      break label$6
     }
    }
    HEAP32[20698] = 0;
    $1 = 1;
    $0 = 27612;
    break label$5;
   }
   while (1) {
    $4 = $1 + 8 | 0;
    $5 = HEAP32[$0 + 40 >> 2];
    $0 = HEAP32[$0 + 8 >> 2];
    $3 = ($5 >>> 0 > $0 >>> 0 ? $5 : $0) + ($3 - $0 | 0) | 0;
    label$10 : {
     if (HEAPU32[$1 + 12 >> 2] >= 2) {
      $0 = __next_zones_zonelist($4, 1, 0);
      break label$10;
     }
     $0 = $4;
    }
    $1 = $0;
    $0 = HEAP32[$1 >> 2];
    if ($0) {
     continue
    }
    break;
   };
   HEAP32[20698] = $3;
   $1 = $3 >>> 0 < 4096;
   $0 = $1 ? 27612 : 27616;
  }
  HEAP32[20703] = $1;
  HEAP32[$2 + 8 >> 2] = $3;
  HEAP32[$2 + 4 >> 2] = $0;
  HEAP32[$2 >> 2] = 1;
  printk(27549, $2);
  global$0 = $2 + 16 | 0;
 }
 
 function build_zonelists() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $4 = HEAP32[35586];
  label$1 : {
   label$2 : {
    if (HEAP32[35457]) {
     HEAP32[35576] = 141788;
     HEAP32[35577] = (141788 - HEAP32[35453] | 0) / 516;
     $0 = 1;
     $2 = $0;
     if (!HEAP32[35328]) {
      break label$1
     }
     break label$2;
    }
    $2 = 0;
    if (!HEAP32[35328]) {
     break label$1
    }
   }
   $1 = ($0 << 3) + 141272 | 0;
   HEAP32[$1 + 1032 >> 2] = 141272;
   HEAP32[$1 + 1036 >> 2] = (141272 - HEAP32[35324] | 0) / 516;
   $2 = $0 + 1 | 0;
  }
  $1 = ($2 << 3) + 142304 | 0;
  if (($4 | 0) <= -1) {
   $6 = (141272 - HEAP32[35324] | 0) / 516 | 0;
   $7 = (141788 - HEAP32[35453] | 0) / 516 | 0;
   $3 = HEAP32[35328];
   $8 = HEAP32[35457];
   $0 = $4;
   while (1) {
    $0 = $0 + 1 | 0;
    if ($0) {
     continue
    }
    break;
   };
   $5 = $1;
   label$6 : {
    label$7 : {
     if ($8) {
      HEAP32[$1 >> 2] = 141788;
      HEAP32[($2 << 3) + 142308 >> 2] = $7;
      $0 = 1;
      $2 = $0;
      if (!$3) {
       break label$6
      }
      break label$7;
     }
     $0 = 0;
     $2 = $0;
     if (!$3) {
      break label$6
     }
    }
    $1 = ($0 << 3) + $1 | 0;
    HEAP32[$1 + 4 >> 2] = $6;
    HEAP32[$1 >> 2] = 141272;
    $2 = $0 + 1 | 0;
   }
   $1 = $5 + ($2 << 3) | 0;
  }
  if (($4 | 0) >= 1) {
   $0 = 0;
   $6 = (141272 - HEAP32[35324] | 0) / 516 | 0;
   $7 = (141788 - HEAP32[35453] | 0) / 516 | 0;
   $2 = HEAP32[35328];
   $8 = HEAP32[35457];
   while (1) {
    if (!$0) {
     label$12 : {
      label$13 : {
       if (!$8) {
        $3 = 0;
        if ($2) {
         break label$13
        }
        break label$12;
       }
       HEAP32[$1 + 4 >> 2] = $7;
       HEAP32[$1 >> 2] = 141788;
       $3 = 1;
       if (!$2) {
        break label$12
       }
      }
      $5 = ($3 << 3) + $1 | 0;
      HEAP32[$5 + 4 >> 2] = $6;
      HEAP32[$5 >> 2] = 141272;
      $3 = $3 + 1 | 0;
     }
     $1 = ($3 << 3) + $1 | 0;
    }
    $0 = $0 + 1 | 0;
    if (($0 | 0) != ($4 | 0)) {
     continue
    }
    break;
   };
  }
  HEAP32[$1 >> 2] = 0;
  HEAP32[$1 + 4 >> 2] = 0;
 }
 
 function build_all_zonelists_init() {
  build_zonelists();
  memset(16904, 0, 36);
  HEAP32[4229] = 16916;
  HEAP32[4231] = 16924;
  HEAP32[4230] = 16916;
  HEAP32[4232] = 16924;
  HEAP32[4234] = 16932;
  HEAP32[4228] = 1;
  HEAP32[4233] = 16932;
  HEAP32[4226] = 0;
  HEAP32[4227] = 0;
  HEAP32[4228] = 1;
 }
 
 function memmap_init_zone($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = $0 + $2 | 0;
  $4 = $3 + -1 | 0;
  if (HEAPU32[20645] < $4 >>> 0) {
   HEAP32[20645] = $4
  }
  if ($3 >>> 0 > $2 >>> 0) {
   $3 = Math_imul($2, 36);
   while (1) {
    $4 = HEAP32[20646] + $3 | 0;
    __init_single_page($4, $2, $1);
    if (!($2 & 1023)) {
     set_pageblock_migratetype($4);
     _cond_resched();
    }
    $3 = $3 + 36 | 0;
    $2 = $2 + 1 | 0;
    $0 = $0 + -1 | 0;
    if ($0) {
     continue
    }
    break;
   };
  }
 }
 
 function __init_single_page($0, $1, $2) {
  memset($0, 0, 36);
  HEAP32[$0 + 28 >> 2] = 1;
  HEAP32[$0 + 24 >> 2] = -1;
  HEAP32[$0 + 32 >> 2] = $1 << 16;
  $1 = $0 + 4 | 0;
  HEAP32[$0 + 8 >> 2] = $1;
  HEAP32[$0 + 4 >> 2] = $1;
  HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & 2147483647 | $2 << 31;
 }
 
 function init_currently_empty_zone($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  HEAP32[$0 + 36 >> 2] = $1;
  $1 = HEAP32[$0 + 24 >> 2];
  HEAP32[$1 + 1056 >> 2] = (($0 - $1 | 0) / 516 | 0) + 1;
  $2 = $0;
  while (1) {
   HEAP32[(Math_imul($3, 36) + $0 | 0) + 92 >> 2] = 0;
   $1 = 0;
   while (1) {
    $5 = $1 + $2 | 0;
    $4 = $5 + 60 | 0;
    HEAP32[$5 - -64 >> 2] = $4;
    HEAP32[$4 >> 2] = $4;
    $1 = $1 + 8 | 0;
    if (($1 | 0) != 32) {
     continue
    }
    break;
   };
   $2 = $2 + 36 | 0;
   $3 = $3 + 1 | 0;
   if (($3 | 0) != 11) {
    continue
   }
   break;
  };
  HEAP32[$0 + 56 >> 2] = 1;
 }
 
 function calculate_node_totalpages($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  while (1) {
   $7 = $0;
   if ($5) {
    $3 = 0;
    $4 = $1;
    while (1) {
     $7 = HEAP32[$4 >> 2] + $7 | 0;
     $4 = $4 + 4 | 0;
     $3 = $3 + 1 | 0;
     if (($3 | 0) != ($5 | 0)) {
      continue
     }
     break;
    };
   }
   $8 = Math_imul($5, 516);
   $11 = $8 + 141320 | 0;
   $3 = $5 << 2;
   $4 = HEAP32[$3 + $1 >> 2];
   $12 = $4;
   if ($2) {
    $3 = HEAP32[$2 + $3 >> 2]
   } else {
    $3 = 0
   }
   $3 = $12 - $3 | 0;
   HEAP32[$11 >> 2] = $3;
   HEAP32[$8 + 141316 >> 2] = $4;
   HEAP32[$8 + 141308 >> 2] = $4 ? $7 : 0;
   $10 = $4 + $10 | 0;
   $9 = $3 + $9 | 0;
   $5 = $5 + 1 | 0;
   if (($5 | 0) != 2) {
    continue
   }
   break;
  };
  HEAP32[35584] = $9;
  HEAP32[35585] = $10;
  HEAP32[$6 + 4 >> 2] = $9;
  HEAP32[$6 >> 2] = HEAP32[35586];
  printk(28109, $6);
  global$0 = $6 + 16 | 0;
 }
 
 function free_area_init_core() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $0 = global$0 - 48 | 0;
  global$0 = $0;
  HEAP32[35588] = 142348;
  HEAP32[35587] = 142348;
  HEAP32[35590] = 142356;
  HEAP32[35589] = 142356;
  lruvec_init();
  HEAP32[35613] = 16940;
  $5 = 28180;
  while (1) {
   $7 = HEAP32[$1 + 141308 >> 2];
   $2 = HEAP32[$1 + 141320 >> 2];
   $6 = HEAP32[$1 + 141316 >> 2];
   $3 = Math_imul($6, 36) + 65535 >>> 16 | 0;
   label$2 : {
    if ($2 >>> 0 < $3 >>> 0) {
     $4 = HEAP32[$5 >> 2];
     HEAP32[$0 + 24 >> 2] = $2;
     HEAP32[$0 + 20 >> 2] = $3;
     HEAP32[$0 + 16 >> 2] = $4;
     printk(28188, $0 + 16 | 0);
     break label$2;
    }
    $2 = $2 - $3 | 0;
    if (!$3) {
     break label$2
    }
    $4 = HEAP32[$5 >> 2];
    HEAP32[$0 + 36 >> 2] = $3;
    HEAP32[$0 + 32 >> 2] = $4;
    printk(28139, $0 + 32 | 0);
   }
   $3 = $1 + 141272 | 0;
   label$4 : {
    if ($1) {
     break label$4
    }
    $4 = HEAP32[35644];
    if ($2 >>> 0 <= $4 >>> 0) {
     break label$4
    }
    HEAP32[$0 >> 2] = 28310;
    HEAP32[$0 + 4 >> 2] = $4;
    printk(28233, $0);
    $2 = $2 - $4 | 0;
   }
   HEAP32[$1 + 141312 >> 2] = $2;
   $4 = HEAP32[$5 >> 2];
   HEAP32[$1 + 141296 >> 2] = 141272;
   HEAP32[$1 + 141324 >> 2] = $4;
   HEAP32[35645] = HEAP32[35645] + $2;
   HEAP32[35646] = HEAP32[35646] + $2;
   zone_pcp_init($3);
   if ($6) {
    setup_usemap($3, $7, $6);
    init_currently_empty_zone($3, $7);
    memmap_init_zone($6, $8, $7);
   }
   $5 = $5 + 4 | 0;
   $8 = $8 + 1 | 0;
   $1 = $1 + 516 | 0;
   if (($1 | 0) != 1032) {
    continue
   }
   break;
  };
  global$0 = $0 + 48 | 0;
 }
 
 function zone_pcp_init($0) {
  var $1 = 0, $2 = 0, $3 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  HEAP32[$0 + 28 >> 2] = 16904;
  $2 = HEAP32[$0 + 48 >> 2];
  if ($2) {
   $3 = HEAP32[$0 + 52 >> 2];
   (wasm2js_i32$0 = $1, wasm2js_i32$1 = zone_batchsize($0)), HEAP32[wasm2js_i32$0 + 8 >> 2] = wasm2js_i32$1;
   HEAP32[$1 + 4 >> 2] = $2;
   HEAP32[$1 >> 2] = $3;
   printk(28317, $1);
  }
  global$0 = $1 + 16 | 0;
 }
 
 function setup_usemap($0, $1, $2) {
  var wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  HEAP32[$0 + 32 >> 2] = 0;
  $1 = ((($1 & 1023) + $2 | 0) + 1023 >>> 8 & 16777212) + 31 >>> 3 & 4194300;
  if ($1) {
   (wasm2js_i32$0 = $0 + 32 | 0, wasm2js_i32$1 = memblock_alloc_try_nid_nopanic($1, 128, HEAP32[35586])), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1
  }
 }
 
 function mem_init_print_info() {
  var $0 = 0, $1 = 0, $2 = 0;
  $0 = global$0 - 48 | 0;
  global$0 = $0;
  HEAP32[$0 + 40 >> 2] = 27757;
  HEAP32[$0 + 36 >> 2] = 27757;
  $1 = HEAP32[20706];
  HEAP32[$0 + 32 >> 2] = $1 << 6;
  HEAP32[$0 + 24 >> 2] = 0;
  HEAP32[$0 + 16 >> 2] = 0;
  $2 = HEAP32[35584];
  HEAP32[$0 + 28 >> 2] = ($2 - HEAP32[20705] | 0) - $1 << 6;
  HEAP32[$0 + 20 >> 2] = 0;
  HEAP32[$0 + 4 >> 2] = $2 << 6;
  HEAP32[$0 >> 2] = HEAP32[20656] << 6;
  HEAP32[$0 + 8 >> 2] = 4194241;
  HEAP32[$0 + 12 >> 2] = 0;
  printk(27619, $0);
  global$0 = $0 + 48 | 0;
 }
 
 function page_alloc_cpu_dead($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  lru_add_drain_cpu();
  $0 = 141272;
  if ($0) {
   while (1) {
    label$3 : {
     if (!HEAP32[$0 + 48 >> 2]) {
      break label$3
     }
     $1 = HEAP32[$0 + 28 >> 2];
     $2 = HEAP32[$1 >> 2];
     if (!$2) {
      break label$3
     }
     free_pcppages_bulk($0, $2, $1);
    }
    $0 = HEAP32[$0 + 24 >> 2] + 516 >>> 0 > $0 >>> 0 ? $0 + 516 | 0 : 0;
    if ($0) {
     continue
    }
    break;
   }
  }
  return 0;
 }
 
 function zone_batchsize($0) {
  var $1 = 0, $2 = 0;
  $0 = HEAP32[$0 + 40 >> 2];
  $0 = ($0 << 6 & -65536) >>> 0 > 1048576 ? 4 : $0 >>> 12 | 0;
  $0 = $0 ? $0 : 1;
  $0 = ($0 >>> 1 | 0) + $0 | 0;
  $1 = $0 >>> 0 > 65535;
  $2 = $1 ? 32 : 16;
  $0 = $1 ? $0 : $0 << 16;
  $1 = $0 >>> 0 > 16777215;
  $2 = $1 ? $2 : $2 + -8 | 0;
  $0 = $1 ? $0 : $0 << 8;
  $1 = $0 >>> 0 > 268435455;
  $2 = $1 ? $2 : $2 + -4 | 0;
  $0 = $1 ? $0 : $0 << 4;
  $1 = $0 >>> 0 > 1073741823;
  return -1 << (($1 ? $2 : $2 + -2 | 0) + (($1 ? $0 : $0 << 2) >> 31 ^ -1) | 0) + -1 ^ -1;
 }
 
 function __probe_kernel_read($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = HEAP32[2];
  HEAP32[$3 + 880 >> 2] = HEAP32[$3 + 880 >> 2] + 1;
  $3 = HEAP32[3];
  HEAP32[3] = 0;
  $4 = HEAP32[2];
  HEAP32[$4 + 444 >> 2] = HEAP32[$4 + 444 >> 2] + 1;
  umem(1, $0 | 0, $1 | 0, $2 | 0) | 0;
  $0 = HEAP32[2];
  HEAP32[$0 + 444 >> 2] = HEAP32[$0 + 444 >> 2] + -1;
  HEAP32[3] = $3;
  $0 = HEAP32[2];
  HEAP32[$0 + 880 >> 2] = HEAP32[$0 + 880 >> 2] + -1;
 }
 
 function oom_badness($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  label$1 : {
   label$2 : {
    if (HEAP32[$0 + 500 >> 2] == 1) {
     break label$2
    }
    if (HEAPU8[$0 + 14 | 0] & 32) {
     break label$2
    }
    $3 = HEAP32[$0 + 732 >> 2];
    $2 = HEAP32[$3 + 12 >> 2];
    if (($2 | 0) == ($3 + 12 | 0)) {
     break label$2
    }
    $3 = $0 + 732 | 0;
    while (1) {
     $0 = HEAP32[$2 + -236 >> 2];
     if (!$0) {
      $2 = HEAP32[$2 >> 2];
      if (($2 | 0) != (HEAP32[$3 >> 2] + 12 | 0)) {
       continue
      }
      break label$2;
     }
     break;
    };
    if (($2 | 0) == 592) {
     break label$2
    }
    $4 = HEAP16[HEAP32[$2 + 140 >> 2] + 362 >> 1];
    if (($4 | 0) == -1e3) {
     break label$1
    }
    if (HEAP32[$0 + 364 >> 2] & 2097152) {
     break label$1
    }
    if (($0 | 0) == HEAP32[HEAP32[$2 + -88 >> 2] + 356 >> 2] ? HEAP32[$2 + 8 >> 2] : 0) {
     break label$1
    }
    $2 = Math_imul($4, ($1 >>> 0) / 1e3 | 0);
    $1 = HEAP32[$0 + 340 >> 2];
    $2 = $2 + (($1 | 0) > 0 ? $1 : 0) | 0;
    $1 = HEAP32[$0 + 344 >> 2];
    $2 = $2 + (($1 | 0) > 0 ? $1 : 0) | 0;
    $1 = HEAP32[$0 + 352 >> 2];
    $2 = $2 + (($1 | 0) > 0 ? $1 : 0) | 0;
    $1 = HEAP32[$0 + 348 >> 2];
    $0 = $2 + (($1 | 0) > 0 ? $1 : 0) + (HEAP32[$0 + 48 >> 2] >>> 16) | 0;
    $4 = ($0 | 0) > 1 ? $0 : 1;
   }
   return $4;
  }
  return 0;
 }
 
 function out_of_memory($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  HEAP32[$4 + 8 >> 2] = 0;
  label$1 : {
   if (HEAPU8[82848]) {
    break label$1
   }
   if (!HEAP32[$0 + 8 >> 2]) {
    blocking_notifier_call_chain($4 + 8 | 0);
    $2 = 1;
    if (HEAP32[$4 + 8 >> 2]) {
     break label$1
    }
   }
   if (task_will_free_mem(HEAP32[2])) {
    $0 = HEAP32[2];
    $2 = HEAP32[$0 + 356 >> 2];
    $3 = HEAP32[$0 + 4 >> 2];
    $1 = HEAP32[$3 >> 2];
    HEAP32[$4 + 12 >> 2] = $1;
    label$4 : {
     if (HEAP32[$4 + 12 >> 2] & 65536) {
      break label$4
     }
     HEAP32[$3 >> 2] = $1 | 65536;
     if ($1 & 65536) {
      break label$4
     }
     $1 = HEAP32[$0 + 732 >> 2];
     if (!HEAP32[$1 + 368 >> 2]) {
      HEAP32[$1 + 368 >> 2] = $2;
      HEAP32[$2 + 44 >> 2] = HEAP32[$2 + 44 >> 2] + 1;
      HEAP32[$2 + 364 >> 2] = HEAP32[$2 + 364 >> 2] | 33554432;
     }
     HEAP32[20711] = HEAP32[20711] + 1;
    }
    $2 = 1;
    if (($0 | 0) == HEAP32[20715]) {
     break label$1
    }
    if (HEAP32[$0 + 884 >> 2]) {
     break label$1
    }
    HEAP32[$0 + 8 >> 2] = HEAP32[$0 + 8 >> 2] + 1;
    HEAP32[$0 + 884 >> 2] = HEAP32[20715];
    HEAP32[20715] = $0;
    __wake_up(17052, 3, 0);
    break label$1;
   }
   $1 = HEAP32[$0 + 12 >> 2];
   if ($1) {
    $2 = 1;
    if (!($1 & 128)) {
     break label$1
    }
   }
   HEAP32[$0 + 4 >> 2] = 0;
   $2 = HEAP32[$0 + 8 >> 2];
   HEAP32[$0 + 20 >> 2] = $2 ? 1 : HEAP32[20705];
   $1 = HEAP32[20714];
   label$7 : {
    if (!$1) {
     break label$7
    }
    if ($2 ? ($1 | 0) != 2 : 0) {
     break label$7
    }
    if (HEAP32[$0 + 16 >> 2] == -1) {
     break label$7
    }
    dump_header($0);
    HEAP32[$4 >> 2] = HEAP32[20714] == 2 ? 28560 : 28571;
    panic(28517, $4);
    abort();
   }
   label$9 : {
    label$10 : {
     label$11 : {
      label$12 : {
       if ($2) {
        break label$12
       }
       if (!HEAP32[20713]) {
        break label$12
       }
       $1 = HEAP32[2];
       if (!HEAP32[$1 + 356 >> 2]) {
        break label$12
       }
       if (HEAP32[$1 + 500 >> 2] == 1) {
        break label$12
       }
       if (HEAPU8[$1 + 14 | 0] & 32) {
        break label$12
       }
       if (HEAPU16[HEAP32[$1 + 732 >> 2] + 362 >> 1] != 64536) {
        break label$11
       }
      }
      if ($2) {
       break label$9
      }
      $2 = HEAP32[343];
      $3 = $2 + -348 | 0;
      if (($3 | 0) == 1024) {
       break label$9
      }
      $9 = $0 + 16 | 0;
      $10 = $0 + 20 | 0;
      $7 = $0 + 28 | 0;
      $6 = $0 + 24 | 0;
      while (1) {
       label$14 : {
        if (HEAP32[$2 + 152 >> 2] == 1) {
         break label$14
        }
        if (HEAPU8[$2 + -334 | 0] & 32) {
         break label$14
        }
        $1 = HEAP32[$2 + 384 >> 2];
        label$15 : {
         if (HEAP32[$9 >> 2] == -1) {
          break label$15
         }
         $5 = HEAP32[$1 + 368 >> 2];
         if (!$5) {
          break label$15
         }
         if (HEAP32[$5 + 364 >> 2] & 2097152) {
          break label$14
         }
         break label$10;
        }
        $5 = -1;
        label$16 : {
         label$17 : {
          if (HEAPU8[$1 + 360 | 0]) {
           break label$17
          }
          $5 = oom_badness($3, HEAP32[$10 >> 2]);
          if (!$5) {
           break label$14
          }
          $1 = HEAP32[$7 >> 2];
          if ($5 >>> 0 < $1 >>> 0) {
           break label$14
          }
          if (($1 | 0) != ($5 | 0)) {
           break label$17
          }
          $1 = HEAP32[$6 >> 2];
          if (HEAP32[$1 + 420 >> 2] > -1) {
           break label$14
          }
          break label$16;
         }
         $1 = HEAP32[$6 >> 2];
        }
        label$18 : {
         if (!$1) {
          break label$18
         }
         $8 = HEAP32[$1 + 8 >> 2] + -1 | 0;
         HEAP32[$1 + 8 >> 2] = $8;
         if ($8) {
          break label$18
         }
         __put_task_struct($1);
        }
        HEAP32[$7 >> 2] = $5;
        HEAP32[$6 >> 2] = $3;
        $2 = $2 + -340 | 0;
        HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + 1;
       }
       $2 = HEAP32[$3 + 348 >> 2];
       $3 = $2 + -348 | 0;
       if (($3 | 0) != 1024) {
        continue
       }
       break;
      };
      break label$9;
     }
     HEAP32[$0 + 24 >> 2] = HEAP32[2];
     $2 = 1;
     HEAP32[$1 + 8 >> 2] = HEAP32[$1 + 8 >> 2] + 1;
     oom_kill_process($0, 28356);
     break label$1;
    }
    $1 = $0 + 24 | 0;
    $2 = HEAP32[$1 >> 2];
    label$19 : {
     if (!$2) {
      break label$19
     }
     $3 = HEAP32[$2 + 8 >> 2] + -1 | 0;
     HEAP32[$2 + 8 >> 2] = $3;
     if ($3) {
      break label$19
     }
     __put_task_struct($2);
    }
    HEAP32[$1 >> 2] = -1;
   }
   HEAP32[$0 + 28 >> 2] = (Math_imul(HEAP32[$0 + 28 >> 2], 1e3) >>> 0) / HEAPU32[$0 + 20 >> 2];
   label$20 : {
    if (HEAP32[$0 + 24 >> 2]) {
     break label$20
    }
    dump_header($0);
    printk(28397, 0);
    if (HEAP32[$0 + 16 >> 2] == -1) {
     break label$20
    }
    if (HEAP32[$0 + 8 >> 2]) {
     break label$20
    }
    panic(28443, 0);
    abort();
   }
   $1 = $0 + 24 | 0;
   $2 = HEAP32[$1 >> 2];
   if ($2 + 1 >>> 0 >= 2) {
    oom_kill_process($0, HEAP32[$0 + 8 >> 2] ? 28489 : 28475);
    $2 = HEAP32[$1 >> 2];
   }
   $2 = ($2 | 0) != 0;
  }
  global$0 = $4 + 16 | 0;
  return $2;
 }
 
 function task_will_free_mem($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  label$1 : {
   label$2 : {
    $2 = HEAP32[$0 + 356 >> 2];
    if (!$2) {
     break label$2
    }
    $4 = HEAP32[$0 + 732 >> 2];
    $1 = HEAP32[$4 + 64 >> 2];
    if ($1 & 8) {
     break label$2
    }
    if (!($1 & 4)) {
     if (HEAP32[$0 + 584 >> 2] != ($0 + 584 | 0)) {
      break label$2
     }
     if (!(HEAPU8[$0 + 12 | 0] & 4)) {
      break label$2
     }
    }
    if (HEAP32[$2 + 364 >> 2] & 2097152) {
     break label$2
    }
    if (HEAP32[$2 + 40 >> 2] < 2) {
     return 1
    }
    $1 = HEAP32[343];
    if (($1 | 0) == 1372) {
     break label$1
    }
    while (1) {
     $3 = HEAP32[$1 + 384 >> 2];
     $5 = $3 + 12 | 0;
     $0 = HEAP32[$3 + 12 >> 2];
     label$6 : {
      if (($5 | 0) == ($0 | 0)) {
       break label$6
      }
      while (1) {
       $6 = HEAP32[$0 + -236 >> 2];
       if (!$6) {
        $0 = HEAP32[$0 >> 2];
        if (($5 | 0) != ($0 | 0)) {
         continue
        }
        break label$6;
       }
       break;
      };
      if (($3 | 0) == ($4 | 0)) {
       break label$6
      }
      if (($2 | 0) != ($6 | 0)) {
       break label$6
      }
      $0 = HEAP32[$3 + 64 >> 2];
      if ($0 & 8) {
       break label$2
      }
      if ($0 & 4) {
       break label$6
      }
      if (HEAP32[$1 + 236 >> 2] != ($1 + 236 | 0)) {
       break label$2
      }
      if (!(HEAPU8[$1 + -336 | 0] & 4)) {
       break label$2
      }
     }
     $1 = HEAP32[$1 >> 2];
     if (($1 | 0) != 1372) {
      continue
     }
     break;
    };
    return 1;
   }
   return 0;
  }
  return 1;
 }
 
 function dump_header($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0;
  $1 = global$0 - 80 | 0;
  global$0 = $1;
  $2 = HEAP32[$0 + 12 >> 2];
  $3 = HEAP32[$0 + 4 >> 2];
  $5 = HEAP32[$0 + 16 >> 2];
  $4 = HEAP32[2];
  HEAP32[$1 + 72 >> 2] = HEAP16[HEAP32[$4 + 732 >> 2] + 362 >> 1];
  HEAP32[$1 + 68 >> 2] = $5;
  HEAP32[$1 - -64 >> 2] = $3;
  HEAP32[$1 + 60 >> 2] = ($3 | 0) != 0;
  HEAP32[$1 + 56 >> 2] = $0 + 12;
  HEAP32[$1 + 52 >> 2] = $2;
  HEAP32[$1 + 48 >> 2] = $4 + 700;
  printk(28793, $1 + 48 | 0);
  if (HEAP32[$0 + 16 >> 2]) {
   printk(28883, 0)
  }
  dump_stack();
  if (!HEAP32[$0 + 8 >> 2]) {
   show_mem(1, HEAP32[$0 + 4 >> 2])
  }
  label$3 : {
   if (!HEAP32[4255]) {
    break label$3
   }
   printk(28912, 0);
   printk(28953, 0);
   $0 = HEAP32[343];
   if (($0 | 0) == 1372) {
    break label$3
   }
   $8 = $1 + 28 | 0;
   $9 = $1 + 20 | 0;
   $10 = $1 + 24 | 0;
   $11 = $1 + 16 | 0;
   while (1) {
    label$5 : {
     if (HEAP32[$0 + 152 >> 2] == 1) {
      break label$5
     }
     if (HEAPU8[$0 + -334 | 0] & 32) {
      break label$5
     }
     $2 = HEAP32[$0 + 384 >> 2];
     $3 = HEAP32[$2 + 12 >> 2];
     if (($3 | 0) == ($2 + 12 | 0)) {
      break label$5
     }
     while (1) {
      $2 = HEAP32[$3 + -236 >> 2];
      if (!$2) {
       $3 = HEAP32[$3 >> 2];
       if (($3 | 0) != (HEAP32[$0 + 384 >> 2] + 12 | 0)) {
        continue
       }
       break label$5;
      }
      break;
     };
     if (($3 | 0) == 592) {
      break label$5
     }
     $12 = HEAP32[$3 + -96 >> 2];
     $13 = HEAP32[$3 + -92 >> 2];
     $5 = HEAP32[$2 + 340 >> 2];
     $4 = HEAP32[$2 + 344 >> 2];
     $6 = HEAP32[$2 + 352 >> 2];
     $14 = HEAP32[$2 + 48 >> 2];
     $7 = HEAP32[$2 + 348 >> 2];
     $2 = HEAP32[$2 + 84 >> 2];
     $15 = HEAP16[HEAP32[$3 + 140 >> 2] + 362 >> 1];
     HEAP32[$1 + 32 >> 2] = $3 + 108;
     HEAP32[$8 >> 2] = $15;
     HEAP32[$9 >> 2] = $14;
     HEAP32[$1 + 4 >> 2] = 0;
     HEAP32[$10 >> 2] = ($7 | 0) > 0 ? $7 : 0;
     HEAP32[$11 >> 2] = ((($4 | 0) > 0 ? $4 : 0) + (($5 | 0) > 0 ? $5 : 0) | 0) + (($6 | 0) > 0 ? $6 : 0);
     HEAP32[$1 + 12 >> 2] = $2;
     HEAP32[$1 + 8 >> 2] = $13;
     HEAP32[$1 >> 2] = $12;
     printk(29039, $1);
    }
    $0 = HEAP32[$0 >> 2];
    if (($0 | 0) != 1372) {
     continue
    }
    break;
   };
  }
  global$0 = $1 + 80 | 0;
 }
 
 function oom_kill_process($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0, $21 = 0, $22 = 0, $23 = 0, $24 = 0, $25 = 0, $26 = 0, $27 = 0, $28 = 0, $29 = 0, $30 = 0, $31 = 0, $32 = 0, $33 = 0, $34 = 0, $35 = 0, $36 = 0, $37 = 0, $38 = 0, $39 = 0;
  $4 = global$0 - 80 | 0;
  global$0 = $4;
  $7 = HEAP32[$0 + 28 >> 2];
  label$1 : {
   label$2 : {
    $2 = HEAP32[$0 + 24 >> 2];
    if (task_will_free_mem($2)) {
     $0 = HEAP32[$2 + 356 >> 2];
     $3 = HEAP32[$2 + 4 >> 2];
     $1 = HEAP32[$3 >> 2];
     HEAP32[$4 + 76 >> 2] = $1;
     label$4 : {
      if (HEAP32[$4 + 76 >> 2] & 65536) {
       break label$4
      }
      HEAP32[$3 >> 2] = $1 | 65536;
      if ($1 & 65536) {
       break label$4
      }
      $1 = HEAP32[$2 + 732 >> 2];
      if (!HEAP32[$1 + 368 >> 2]) {
       HEAP32[$1 + 368 >> 2] = $0;
       HEAP32[$0 + 44 >> 2] = HEAP32[$0 + 44 >> 2] + 1;
       HEAP32[$0 + 364 >> 2] = HEAP32[$0 + 364 >> 2] | 33554432;
      }
      HEAP32[20711] = HEAP32[20711] + 1;
     }
     label$6 : {
      if (HEAP32[20715] == ($2 | 0)) {
       break label$6
      }
      if (HEAP32[$2 + 884 >> 2]) {
       break label$6
      }
      HEAP32[$2 + 8 >> 2] = HEAP32[$2 + 8 >> 2] + 1;
      HEAP32[$2 + 884 >> 2] = HEAP32[20715];
      HEAP32[20715] = $2;
      __wake_up(17052, 3, 0);
     }
     break label$2;
    }
    if (___ratelimit(17060, 28583)) {
     dump_header($0)
    }
    $9 = HEAP32[$2 + 496 >> 2];
    HEAP32[$4 + 60 >> 2] = $7;
    HEAP32[$4 + 56 >> 2] = $2 + 700;
    HEAP32[$4 + 52 >> 2] = $9;
    HEAP32[$4 + 48 >> 2] = $1;
    printk(28600, $4 + 48 | 0);
    label$8 : {
     label$9 : {
      $1 = HEAP32[$2 + 732 >> 2];
      $9 = HEAP32[$1 + 12 >> 2];
      if (($9 | 0) != ($1 + 12 | 0)) {
       $20 = -80;
       $21 = -520;
       $22 = $2 + 356 | 0;
       $23 = $0 + 20 | 0;
       $24 = -1;
       $25 = -512;
       $26 = 1;
       $27 = $2 + 732 | 0;
       $28 = -236;
       break label$9;
      }
      $0 = 0;
      break label$8;
     }
     $0 = 1;
    }
    while (1) {
     label$11 : {
      label$13 : {
       label$14 : {
        label$15 : {
         label$16 : {
          label$17 : {
           label$18 : {
            label$19 : {
             label$20 : {
              label$21 : {
               if (!$0) {
                $3 = HEAP32[$2 + 732 >> 2];
                $0 = HEAP32[$3 + 12 >> 2];
                if (($0 | 0) == ($3 + 12 | 0)) {
                 break label$2
                }
                $3 = $2 + 732 | 0;
                while (1) {
                 if (!HEAP32[$0 + -236 >> 2]) {
                  $0 = HEAP32[$0 >> 2];
                  if (($0 | 0) != (HEAP32[$3 >> 2] + 12 | 0)) {
                   continue
                  }
                  break label$2;
                 }
                 break;
                };
                label$24 : {
                 $3 = $0 + -592 | 0;
                 if ($3) {
                  break label$24
                 }
                 break label$2;
                }
                label$28 : {
                 if (($2 | 0) == ($3 | 0)) {
                  $3 = $2;
                  break label$28;
                 }
                 $0 = $0 + -584 | 0;
                 HEAP32[$0 >> 2] = HEAP32[$0 >> 2] + 1;
                 $0 = HEAP32[$2 + 8 >> 2] + -1 | 0;
                 HEAP32[$2 + 8 >> 2] = $0;
                 if ($0) {
                  break label$28
                 }
                 __put_task_struct($2);
                }
                $14 = 1;
                $10 = HEAP32[$3 + 356 >> 2];
                HEAP32[$10 + 44 >> 2] = HEAP32[$10 + 44 >> 2] + 1;
                do_send_sig_info(9, 1, $3, 1);
                $0 = HEAP32[$3 + 356 >> 2];
                $6 = HEAP32[$3 + 4 >> 2];
                $5 = HEAP32[$6 >> 2];
                HEAP32[$4 + 76 >> 2] = $5;
                label$30 : {
                 if (HEAP32[$4 + 76 >> 2] & 65536) {
                  break label$30
                 }
                 HEAP32[$6 >> 2] = $5 | 65536;
                 if ($5 & 65536) {
                  break label$30
                 }
                 $5 = HEAP32[$3 + 732 >> 2];
                 if (!HEAP32[$5 + 368 >> 2]) {
                  HEAP32[$5 + 368 >> 2] = $0;
                  HEAP32[$0 + 44 >> 2] = HEAP32[$0 + 44 >> 2] + 1;
                  HEAP32[$0 + 364 >> 2] = HEAP32[$0 + 364 >> 2] | 33554432;
                 }
                 HEAP32[20711] = HEAP32[20711] + 1;
                }
                $8 = HEAP32[$3 + 496 >> 2];
                $5 = HEAP32[$0 + 344 >> 2];
                $29 = 352;
                $6 = HEAP32[$0 + 352 >> 2];
                $7 = HEAP32[$0 + 84 >> 2];
                $0 = HEAP32[$0 + 340 >> 2];
                HEAP32[$4 + 32 >> 2] = (($0 | 0) > 0 ? $0 : 0) << 6;
                HEAP32[$4 + 36 >> 2] = (($6 | 0) > 0 ? $6 : 0) << 6;
                HEAP32[$4 + 24 >> 2] = $7 << 6;
                $15 = $3 + 700 | 0;
                HEAP32[$4 + 20 >> 2] = $15;
                HEAP32[$4 + 16 >> 2] = $8;
                HEAP32[$4 + 28 >> 2] = (($5 | 0) > 0 ? $5 : 0) << 6;
                printk(28656, $4 + 16 | 0);
                $16 = -348;
                $17 = 1024;
                $5 = HEAP32[343];
                $8 = $5 + -348 | 0;
                if (($8 | 0) == 1024) {
                 break label$21
                }
                $13 = 1;
                $30 = 12;
                $31 = -236;
                $32 = $3 + 732 | 0;
                $18 = $10 + 364 | 0;
                $33 = 2097152;
                $34 = $3 + 496 | 0;
                $35 = 28746;
                $36 = -334;
                $37 = 32;
                $38 = 9;
                $6 = 1;
                break label$13;
               }
               if (!$6) {
                $11 = $9 + $20 | 0;
                $7 = HEAP32[$11 >> 2];
                if (($7 | 0) != ($11 | 0)) {
                 while (1) {
                  $0 = HEAP32[$7 + 212 >> 2];
                  $1 = $0 + 12 | 0;
                  $0 = HEAP32[$0 + 12 >> 2];
                  label$36 : {
                   label$37 : {
                    if (($1 | 0) == ($0 | 0)) {
                     break label$37
                    }
                    $39 = HEAP32[$22 >> 2];
                    while (1) {
                     $19 = HEAP32[$0 + $28 >> 2];
                     if (!$19) {
                      $0 = HEAP32[$0 >> 2];
                      if (($1 | 0) != ($0 | 0)) {
                       continue
                      }
                      break label$37;
                     }
                     break;
                    };
                    if (($39 | 0) == ($19 | 0)) {
                     break label$36
                    }
                   }
                   $0 = $7 + $21 | 0;
                   $1 = oom_badness($0, HEAP32[$23 >> 2]);
                   if ($1 >>> 0 <= $12 >>> 0) {
                    break label$36
                   }
                   $12 = HEAP32[$2 + 8 >> 2] + $24 | 0;
                   HEAP32[$2 + 8 >> 2] = $12;
                   if (!$12) {
                    __put_task_struct($2)
                   }
                   $2 = $7 + $25 | 0;
                   HEAP32[$2 >> 2] = HEAP32[$2 >> 2] + $26;
                   $12 = $1;
                   $2 = $0;
                  }
                  $7 = HEAP32[$7 >> 2];
                  if (($11 | 0) != ($7 | 0)) {
                   continue
                  }
                  break;
                 };
                 $1 = HEAP32[$27 >> 2];
                }
                $9 = HEAP32[$9 >> 2];
                if (($9 | 0) != ($1 + 12 | 0)) {
                 break label$19
                }
                $0 = 0;
                continue;
               }
               $6 = HEAP32[$5 + 384 >> 2];
               $7 = $6 + $30 | 0;
               label$41 : {
                $0 = HEAP32[$6 + 12 >> 2];
                label$42 : {
                 if (($7 | 0) == ($0 | 0)) {
                  break label$42
                 }
                 while (1) {
                  $11 = HEAP32[$0 + $31 >> 2];
                  if (!$11) {
                   $0 = HEAP32[$0 >> 2];
                   if (($7 | 0) != ($0 | 0)) {
                    continue
                   }
                   break label$42;
                  }
                  break;
                 };
                 if (($10 | 0) != ($11 | 0)) {
                  break label$42
                 }
                 if (($6 | 0) == HEAP32[$32 >> 2]) {
                  break label$42
                 }
                 if (HEAP32[$5 + 152 >> 2] == ($13 | 0)) {
                  break label$41
                 }
                 if (HEAPU8[$5 + $36 | 0] & $37) {
                  break label$42
                 }
                 do_send_sig_info($38, $13, $8, $13);
                }
                $5 = HEAP32[$8 + 348 >> 2];
                $8 = $5 + $16 | 0;
                if (($8 | 0) != ($17 | 0)) {
                 break label$18
                }
                break label$21;
               }
               HEAP32[$18 >> 2] = HEAP32[$18 >> 2] | $33;
               $0 = HEAP32[$34 >> 2];
               $6 = HEAP32[$5 + 148 >> 2];
               HEAP32[$4 + 12 >> 2] = $5 + $29;
               HEAP32[$4 + 8 >> 2] = $6;
               HEAP32[$4 + 4 >> 2] = $15;
               HEAP32[$4 >> 2] = $0;
               printk($35, $4);
               $14 = 0;
               $5 = HEAP32[$8 + 348 >> 2];
               $8 = $5 + $16 | 0;
               if (($8 | 0) != ($17 | 0)) {
                break label$17
               }
               break label$20;
              }
              if (HEAP32[20715] == ($3 | 0)) {
               break label$20
              }
              if (!$14) {
               break label$20
              }
              if (HEAP32[$3 + 884 >> 2]) {
               break label$20
              }
              HEAP32[$3 + 8 >> 2] = HEAP32[$3 + 8 >> 2] + 1;
              HEAP32[$3 + 884 >> 2] = HEAP32[20715];
              HEAP32[20715] = $3;
              __wake_up(17052, 3, 0);
             }
             $1 = $10 + 44 | 0;
             $0 = HEAP32[$1 >> 2] + -1 | 0;
             HEAP32[$1 >> 2] = $0;
             if ($0) {
              break label$11
             }
             __mmdrop($10);
             break label$11;
            }
            $6 = 0;
            break label$16;
           }
           $6 = 1;
           break label$15;
          }
          $6 = 1;
          break label$14;
         }
         $0 = 1;
         continue;
        }
        $0 = 1;
        continue;
       }
       $0 = 1;
       continue;
      }
      $0 = 1;
      continue;
     }
     break;
    };
    $0 = HEAP32[$3 + 8 >> 2] + -1 | 0;
    HEAP32[$3 + 8 >> 2] = $0;
    if ($0) {
     break label$1
    }
    __put_task_struct($3);
    break label$1;
   }
   $0 = HEAP32[$2 + 8 >> 2] + -1 | 0;
   HEAP32[$2 + 8 >> 2] = $0;
   if ($0) {
    break label$1
   }
   __put_task_struct($2);
  }
  global$0 = $4 + 80 | 0;
 }
 
 function __delete_from_page_cache($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $2 = global$0 - 48 | 0;
  global$0 = $2;
  $3 = HEAP32[$0 + 12 >> 2];
  unaccount_page_cache_page($3, $0);
  HEAP32[$2 + 24 >> 2] = $3 + 4;
  HEAP32[$2 + 44 >> 2] = 91;
  HEAP32[$2 + 36 >> 2] = 3;
  HEAP32[$2 + 40 >> 2] = 0;
  HEAP32[$2 + 32 >> 2] = 0;
  $4 = HEAP32[$0 + 16 >> 2];
  HEAP32[$2 + 28 >> 2] = $4;
  if (!(HEAPU8[$0 + 45 | 0] ? !!(HEAP32[$0 >> 2] & 65536) : 0)) {
   HEAP32[$2 + 36 >> 2] = 3;
   HEAP32[$2 + 28 >> 2] = $4;
   $4 = HEAP32[$0 >> 2] & 65536 ? HEAPU8[$0 + 45 | 0] : 0;
   xas_store($2 + 24 | 0, $1);
   xas_init_marks($2 + 24 | 0);
   HEAP32[$0 + 12 >> 2] = 0;
   $0 = 1 << $4;
   if ($1) {
    HEAP32[$3 + 44 >> 2] = $0 + HEAP32[$3 + 44 >> 2]
   }
   HEAP32[$3 + 40 >> 2] = HEAP32[$3 + 40 >> 2] - $0;
   global$0 = $2 + 48 | 0;
   return;
  }
  HEAP32[$2 + 8 >> 2] = 29280;
  HEAP32[$2 + 4 >> 2] = 1230;
  HEAP32[$2 >> 2] = 29227;
  printk(29092, $2);
  abort();
 }
 
 function unaccount_page_cache_page($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $2 = HEAP32[$1 + 4 >> 2];
  if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $1) >> 2] & 4) {
   $2 = HEAP32[$1 + 4 >> 2];
   $2 & 1 ? $2 + -1 | 0 : $1;
  }
  label$2 : {
   if (!page_mapped($1)) {
    break label$2
   }
   HEAP32[$3 >> 2] = HEAP32[2] + 700;
   HEAP32[$3 + 4 >> 2] = ($1 - HEAP32[20646] | 0) / 36;
   printk(29153, $3);
   __dump_page($1, 29201);
   dump_stack();
   add_taint(5, 1);
   label$3 : {
    label$4 : {
     if (HEAP32[$1 >> 2] & 65536) {
      break label$4
     }
     if (HEAP32[$1 + 4 >> 2] & 1) {
      break label$4
     }
     $2 = HEAP32[$1 + 24 >> 2] + 1 | 0;
     break label$3;
    }
    $2 = __page_mapcount($1);
   }
   if (!(HEAP32[$0 + 56 >> 2] & 16)) {
    break label$2
   }
   $4 = HEAP32[$1 + 4 >> 2];
   if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) + 28 >> 2] < ($2 + 2 | 0)) {
    break label$2
   }
   HEAP32[$1 + 24 >> 2] = -1;
   HEAP32[$1 + 28 >> 2] = HEAP32[$1 + 28 >> 2] - $2;
  }
  HEAP32[35630] = HEAP32[35630] + -1;
  HEAP32[20684] = HEAP32[20684] + -1;
  $2 = $1 + 4 | 0;
  $4 = HEAP32[$2 >> 2];
  if (HEAP32[($4 & 1 ? $4 + -1 | 0 : $1) >> 2] & 524288) {
   HEAP32[35634] = HEAP32[35634] + -1;
   HEAP32[20688] = HEAP32[20688] + -1;
  }
  $2 = HEAP32[$2 >> 2];
  if (HEAP32[($2 & 1 ? $2 + -1 | 0 : $1) >> 2] & 8) {
   $2 = $0;
   $0 = HEAP32[$0 >> 2];
   label$7 : {
    if ($0) {
     $0 = HEAP32[HEAP32[$0 + 20 >> 2] + 108 >> 2];
     break label$7;
    }
    $0 = 16184;
   }
   account_page_cleaned($1, $2, $0 + 56 | 0);
  }
  global$0 = $3 + 16 | 0;
 }
 
 function wake_page_function($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0, $5 = 0, $6 = 0;
  label$1 : {
   $5 = HEAP32[$0 + -8 >> 2];
   label$2 : {
    if (($5 | 0) != HEAP32[$3 >> 2]) {
     break label$2
    }
    HEAP32[$3 + 8 >> 2] = 1;
    $4 = HEAP32[$0 + -4 >> 2];
    if (($4 | 0) != HEAP32[$3 + 4 >> 2]) {
     break label$2
    }
    $6 = -1;
    if (!(HEAP32[(($4 | 0) / 32 << 2) + $5 >> 2] & 1 << ($4 & 31))) {
     break label$1
    }
   }
   return $6 | 0;
  }
  return autoremove_wake_function($0, $1, $2, $3) | 0;
 }
 
 function unlock_page($0) {
  var $1 = 0;
  $1 = HEAP32[$0 + 4 >> 2];
  $0 = $1 & 1 ? $1 + -1 | 0 : $0;
  $1 = HEAP32[$0 >> 2];
  HEAP32[$0 >> 2] = $1 & -2;
  if ($1 & 128) {
   wake_up_page_bit($0)
  }
 }
 
 function wake_up_page_bit($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 48 | 0;
  global$0 = $1;
  $2 = $1 + 20 | 0;
  HEAP32[$1 + 24 >> 2] = $2;
  HEAP32[$1 + 40 >> 2] = 0;
  HEAP32[$1 + 36 >> 2] = 0;
  HEAP32[$1 + 32 >> 2] = $0;
  HEAP32[$1 + 20 >> 2] = $2;
  HEAP32[$1 + 16 >> 2] = 0;
  HEAP32[$1 + 8 >> 2] = 0;
  HEAP32[$1 + 12 >> 2] = 0;
  $2 = HEAP32[20716];
  HEAP32[20716] = 0;
  $3 = (Math_imul($0, 1640531527) >>> 24 << 3) + 17152 | 0;
  __wake_up_locked_key_bookmark($3, $1 + 32 | 0, $1 + 8 | 0);
  if (HEAPU8[$1 + 8 | 0] & 4) {
   while (1) {
    HEAP32[20716] = $2;
    $2 = HEAP32[20716];
    HEAP32[20716] = 0;
    __wake_up_locked_key_bookmark($3, $1 + 32 | 0, $1 + 8 | 0);
    if (HEAPU8[$1 + 8 | 0] & 4) {
     continue
    }
    break;
   }
  }
  if (!(HEAP32[$1 + 40 >> 2] ? HEAP32[$3 >> 2] != ($3 | 0) : 0)) {
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & -129
  }
  HEAP32[20716] = $2;
  global$0 = $1 + 48 | 0;
 }
 
 function __lock_page($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $3 = global$0 - 32 | 0;
  global$0 = $3;
  $2 = HEAP32[$0 + 4 >> 2];
  $0 = $2 & 1 ? $2 + -1 | 0 : $0;
  $5 = Math_imul($0, 1640531527) >>> 24 | 0;
  $4 = HEAP32[$0 + 4 >> 2];
  label$1 : {
   if (!(HEAP32[($4 & 1 ? $4 + -1 | 0 : $0) >> 2] & 4)) {
    $1 = HEAP32[$0 + 4 >> 2];
    $4 = 0;
    if (!(HEAP32[($1 & 1 ? $1 + -1 | 0 : $0) >> 2] & 64)) {
     break label$1
    }
    $1 = HEAP32[$0 + 4 >> 2];
    $1 & 1 ? $1 + -1 | 0 : $0;
    $4 = 1;
    break label$1;
   }
   $4 = 0;
  }
  $7 = $3 + 8 | 0;
  HEAP32[$3 + 12 >> 2] = HEAP32[2];
  $6 = $3 + 24 | 0;
  $2 = $3 + 20 | 0;
  HEAP32[$6 >> 2] = $2;
  HEAP32[$3 + 16 >> 2] = 92;
  HEAP32[$3 + 4 >> 2] = 0;
  HEAP32[$3 + 8 >> 2] = 1;
  HEAP32[$3 >> 2] = $0;
  HEAP32[$2 >> 2] = $2;
  $1 = $5 << 3;
  $5 = $1 + 17156 | 0;
  $8 = $1 + 17152 | 0;
  while (1) {
   HEAP32[20716] = 0;
   if (HEAP32[$2 >> 2] == ($2 | 0)) {
    $1 = HEAP32[$5 >> 2];
    HEAP32[$1 >> 2] = $2;
    HEAP32[$6 >> 2] = $1;
    HEAP32[$2 >> 2] = $8;
    HEAP32[$5 >> 2] = $2;
    HEAP32[$0 >> 2] = HEAP32[$0 >> 2] | 128;
   }
   HEAP32[$3 + 28 >> 2] = 2;
   HEAP32[HEAP32[2] >> 2] = 2;
   HEAP32[20716] = 1;
   if (HEAP32[$0 >> 2] & 1) {
    io_schedule()
   }
   $1 = HEAP32[$0 >> 2];
   HEAP32[$3 + 28 >> 2] = $1;
   if (HEAP32[$3 + 28 >> 2] & 1) {
    continue
   }
   HEAP32[$0 >> 2] = $1 | 1;
   if ($1 & 1) {
    continue
   }
   break;
  };
  finish_wait($7);
  if ($4) {
   $2 = HEAP32[$0 + 4 >> 2];
   $2 & 1 ? $2 + -1 | 0 : $0;
  }
  global$0 = $3 + 32 | 0;
 }
 
 function try_to_release_page($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  $4 = HEAP32[$0 + 12 >> 2];
  $3 = HEAP32[$0 + 4 >> 2];
  if (HEAP32[($3 & 1 ? $3 + -1 | 0 : $0) >> 2] & 1) {
   $3 = 0;
   $5 = HEAP32[$0 + 4 >> 2];
   label$2 : {
    if (HEAP32[($5 & 1 ? $5 + -1 | 0 : $0) >> 2] & 32768) {
     break label$2
    }
    $3 = 1;
    if (!$4) {
     break label$2
    }
    $4 = HEAP32[HEAP32[$4 + 52 >> 2] + 36 >> 2];
    if (!$4) {
     break label$2
    }
    $3 = FUNCTION_TABLE[$4]($0, $1) | 0;
   }
   global$0 = $2 + 16 | 0;
   return $3;
  }
  HEAP32[$2 + 8 >> 2] = 29133;
  HEAP32[$2 + 4 >> 2] = 3323;
  HEAP32[$2 >> 2] = 29123;
  printk(29092, $2);
  abort();
 }
 
 function wb_wakeup($0) {
  var $1 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  if (HEAP32[$0 + 4 >> 2] & 1) {
   mod_delayed_work_on(HEAP32[20651], $0 + 144 | 0)
  }
  printk(29294, 0);
  HEAP32[$1 + 8 >> 2] = 29428;
  HEAP32[$1 + 4 >> 2] = 382;
  HEAP32[$1 >> 2] = 29373;
  printk(29342, $1);
  abort();
 }
 
 function wakeup_flusher_threads() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  label$1 : {
   $0 = HEAP32[4124];
   if (($0 | 0) != 16496) {
    while (1) {
     label$4 : {
      if (!HEAP32[$0 + 48 >> 2]) {
       break label$4
      }
      $5 = $0 + 256 | 0;
      $1 = HEAP32[$0 + 256 >> 2];
      if (($5 | 0) == ($1 | 0)) {
       break label$4
      }
      while (1) {
       $3 = $1 + -188 | 0;
       label$6 : {
        if (!(HEAP32[$3 >> 2] & 4)) {
         break label$6
        }
        if (HEAP32[$3 >> 2] & 8) {
         break label$6
        }
        $4 = HEAP32[$3 >> 2];
        HEAP32[$2 + 12 >> 2] = $4;
        if (HEAP32[$2 + 12 >> 2] & 8) {
         break label$6
        }
        HEAP32[$3 >> 2] = $4 | 8;
        if (!($4 & 8)) {
         break label$1
        }
       }
       $1 = HEAP32[$1 >> 2];
       if (($1 | 0) != ($5 | 0)) {
        continue
       }
       break;
      };
     }
     $0 = HEAP32[$0 >> 2];
     if (($0 | 0) != 16496) {
      continue
     }
     break;
    }
   }
   global$0 = $2 + 16 | 0;
   return;
  }
  HEAP32[$1 + -60 >> 2] = 1;
  wb_wakeup($1 + -192 | 0);
  abort();
 }
 
 function sys_m_read($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  return sys_read_3($0, $1, $2) | 0;
 }
 
 function sys_read_3($0, $1, $2) {
  var $3 = 0;
  $3 = global$0;
  $2 = $3 - ($2 + 15 & -16) | 0;
  global$0 = $2;
  $0 = read($0, $2);
  kmem(1, $1 | 0, $2 | 0, $0 | 0) | 0;
  global$0 = $3;
  return $0;
 }
 
 function sys_m_writev($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  return sys_writev_3($0, $1, $2) | 0;
 }
 
 function sys_writev_3($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $3 = global$0 - 256 | 0;
  $5 = $3;
  global$0 = $3;
  $3 = $2 << 3;
  $4 = $5 - ($3 + 15 & -16) | 0;
  global$0 = $4;
  umem(1, $4 | 0, $1 | 0, $3 | 0) | 0;
  if ($2) {
   $1 = $5;
   $3 = $4;
   $6 = $2;
   while (1) {
    $7 = HEAP32[$3 + 4 >> 2];
    umem(1, $1 | 0, HEAP32[$3 >> 2], $7 | 0) | 0;
    HEAP32[$3 >> 2] = $1;
    $3 = $3 + 8 | 0;
    $1 = $1 + $7 | 0;
    $6 = $6 + -1 | 0;
    if ($6) {
     continue
    }
    break;
   };
  }
  $0 = writev($0, $4, $2);
  global$0 = $5 + 256 | 0;
  return $0;
 }
 
 function read($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  if ($0) {
   return -9
  }
  $3 = HEAP32[7800];
  $2 = HEAP32[$3 + 128 >> 2];
  $0 = $2;
  $4 = HEAP32[$0 + 4 >> 2];
  $0 = HEAP32[$0 >> 2];
  if (!(($0 | 0) != 0 | ($4 | 0) != 0)) {
   while (1) {
    do_events();
    $0 = HEAP32[$2 >> 2];
    if (!($0 | HEAP32[$2 + 4 >> 2])) {
     continue
    }
    break;
   }
  }
  memcpy($1, ($2 + HEAP32[$3 + 56 >> 2] | 0) + 8 | 0, $0);
  HEAP32[$2 >> 2] = 0;
  HEAP32[$2 + 4 >> 2] = 0;
  $1 = HEAP32[$3 + 60 >> 2] + ($0 >> 31) | 0;
  $2 = HEAP32[$3 + 56 >> 2] + $0 | 0;
  HEAP32[$3 + 56 >> 2] = $2;
  HEAP32[$3 + 60 >> 2] = $2 >>> 0 < $0 >>> 0 ? $1 + 1 | 0 : $1;
  return $0;
 }
 
 function write($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  label$1 : {
   label$2 : {
    if (($0 | 0) != 1) {
     $3 = -9;
     if ($0) {
      break label$1
     }
     $4 = HEAP32[7800];
     $0 = HEAP32[$4 + 128 >> 2];
     memcpy(($0 + HEAP32[$4 + 56 >> 2] | 0) + 8 | 0, $1, $2);
     $1 = HEAP32[$0 + 4 >> 2];
     $4 = $2 + HEAP32[$0 >> 2] | 0;
     if ($4 >>> 0 < $2 >>> 0) {
      $1 = $1 + 1 | 0
     }
     HEAP32[$0 >> 2] = $4;
     HEAP32[$0 + 4 >> 2] = $1;
     break label$2;
    }
    $4 = HEAP32[7801];
    $5 = HEAP32[$4 + 128 >> 2];
    $0 = HEAP32[$4 + 60 >> 2];
    $3 = HEAP32[$4 + 56 >> 2];
    $6 = $2 + $3 | 0;
    if ($6 >>> 0 < $2 >>> 0) {
     $0 = $0 + 1 | 0
    }
    $7 = $6;
    if (($0 | 0) < 0) {
     $0 = 1
    } else {
     if (($0 | 0) <= 0) {
      $0 = $7 >>> 0 >= 4097 ? 0 : 1
     } else {
      $0 = 0
     }
    }
    if (!$0) {
     flush($5 | 0, $3 | 0);
     $0 = $4 + 56 | 0;
     HEAP32[$0 >> 2] = 0;
     HEAP32[$0 + 4 >> 2] = 0;
     $3 = 0;
    }
    memcpy($3 + $5 | 0, $1, $2);
    $3 = $4 + 56 | 0;
    $7 = $3;
    $8 = $3;
    $5 = HEAP32[$3 + 4 >> 2];
    $0 = $2;
    $6 = $0 + HEAP32[$3 >> 2] | 0;
    if ($6 >>> 0 < $0 >>> 0) {
     $5 = $5 + 1 | 0
    }
    $0 = $6;
    HEAP32[$8 >> 2] = $0;
    HEAP32[$7 + 4 >> 2] = $5;
    if (HEAPU8[($1 + $2 | 0) + -1 | 0]) {
     break label$2
    }
    flush(HEAP32[$4 + 128 >> 2], $0 | 0);
    HEAP32[$3 >> 2] = 0;
    HEAP32[$3 + 4 >> 2] = 0;
   }
   $3 = $2;
  }
  return $3;
 }
 
 function writev($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  label$1 : {
   label$2 : {
    if (($2 | 0) >= 1) {
     $5 = ($2 << 3) + $1 | 0;
     $2 = 0;
     while (1) {
      $4 = HEAP32[$1 + 4 >> 2];
      $3 = write($0, HEAP32[$1 >> 2], $4);
      if (($3 | 0) < 0) {
       break label$2
      }
      if ($3 >>> 0 < $4 >>> 0) {
       break label$1
      }
      $2 = $2 + $4 | 0;
      $1 = $1 + 8 | 0;
      if ($1 >>> 0 < $5 >>> 0) {
       continue
      }
      break;
     };
     return $2;
    }
    return 0;
   }
   return $3;
  }
  return $2 + $3 | 0;
 }
 
 function do_shutdown() {
  var $0 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  (wasm2js_i32$0 = 19424, wasm2js_i32$1 = (js_emem(HEAP32[4856], $0 + 15 | 0, 1) | 0) + HEAP32[4856] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  js_shutdown(HEAPU8[$0 + 15 | 0]);
  global$0 = $0 + 16 | 0;
 }
 
 function do_events() {
  var $0 = 0, $1 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  $1 = evt_count(100) | 0;
  if (($1 | 0) >= 1) {
   while (1) {
    (wasm2js_i32$0 = 19424, wasm2js_i32$1 = (js_emem(HEAP32[4856], $0 + 15 | 0, 1) | 0) + HEAP32[4856] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
    FUNCTION_TABLE[HEAP32[(HEAPU8[$0 + 15 | 0] << 2) + 29456 >> 2]]();
    $1 = $1 + -1 | 0;
    if ($1) {
     continue
    }
    break;
   }
  }
  global$0 = $0 + 16 | 0;
 }
 
 function evt_loop() {
  var $0 = 0, $1 = 0, $2 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  while (1) {
   $1 = evt_count(100) | 0;
   if (($1 | 0) >= 1) {
    $2 = $1;
    while (1) {
     (wasm2js_i32$0 = 19424, wasm2js_i32$1 = (js_emem(HEAP32[4856], $0 + 15 | 0, 1) | 0) + HEAP32[4856] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
     FUNCTION_TABLE[HEAP32[(HEAPU8[$0 + 15 | 0] << 2) + 29456 >> 2]]();
     $2 = $2 + -1 | 0;
     if ($2) {
      continue
     }
     break;
    };
   }
   if (!$1) {
    continue
   }
   break;
  };
  global$0 = $0 + 16 | 0;
 }
 
 function do_write() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $1 = global$0 - 8208 | 0;
  global$0 = $1;
  $0 = (js_emem(HEAP32[4856], $1 + 8207 | 0, 1) | 0) + HEAP32[4856] | 0;
  HEAP32[4856] = $0;
  $2 = HEAPU8[$1 + 8207 | 0];
  $0 = (js_emem($0 | 0, $1 + 8207 | 0, 1) | 0) + HEAP32[4856] | 0;
  HEAP32[4856] = $0;
  $3 = $0;
  $0 = HEAPU8[$1 + 8207 | 0] << 8 | $2;
  $0 = (($0 & 65535) >>> 0 < 8192 ? $0 : 8192) & 65535;
  (wasm2js_i32$0 = 19424, wasm2js_i32$1 = (js_emem($3 | 0, $1 | 0, $0 | 0) | 0) + HEAP32[4856] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  if ($0) {
   HEAP8[$0 + $1 | 0] = 0;
   write(0, $1, $0 + 1 | 0);
  }
  global$0 = $1 + 8208 | 0;
 }
 
 function do_run() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $1 = global$0 - 272 | 0;
  global$0 = $1;
  $0 = (js_emem(HEAP32[4856], $1 + 271 | 0, 1) | 0) + HEAP32[4856] | 0;
  HEAP32[4856] = $0;
  $2 = HEAPU8[$1 + 271 | 0];
  $0 = (js_emem($0 | 0, $1 + 271 | 0, 1) | 0) + HEAP32[4856] | 0;
  HEAP32[4856] = $0;
  $3 = $0;
  $0 = $2 | HEAPU8[$1 + 271 | 0] << 8;
  $2 = (($0 & 65535) >>> 0 < 256 ? $0 : 256) & 65535;
  (wasm2js_i32$0 = 19424, wasm2js_i32$1 = (js_emem($3 | 0, $1 | 0, $2 | 0) | 0) + HEAP32[4856] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  $0 = HEAP32[7801];
  flush(HEAP32[$0 + 128 >> 2], HEAP32[$0 + 56 >> 2]);
  HEAP32[$0 + 56 >> 2] = 0;
  HEAP32[$0 + 60 >> 2] = 0;
  js_run($1 | 0, $2 | 0);
  global$0 = $1 + 272 | 0;
 }
 
 function sys_m_ioctl($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  HEAP32[$3 + 12 >> 2] = $2;
  HEAP32[$3 + 8 >> 2] = $1;
  HEAP32[$3 + 4 >> 2] = $0;
  HEAP32[$3 >> 2] = 3;
  printk(29484, $3);
  global$0 = $3 + 16 | 0;
  return 0;
 }
 
 function __syscall0($0) {
  $0 = $0 | 0;
  var $1 = 0, $2 = 0;
  $1 = global$0 - 16 | 0;
  global$0 = $1;
  $2 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($2 | 0) != 96) {
    $0 = FUNCTION_TABLE[$2]() | 0;
    break label$1;
   }
   HEAP32[$1 + 4 >> 2] = $0;
   HEAP32[$1 >> 2] = 0;
   printk(30700, $1);
   $0 = -38;
  }
  global$0 = $1 + 16 | 0;
  return $0 | 0;
 }
 
 function __syscall1($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  var $2 = 0, $3 = 0;
  $2 = global$0 - 16 | 0;
  global$0 = $2;
  $3 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($3 | 0) != 96) {
    $0 = FUNCTION_TABLE[$3]($1) | 0;
    break label$1;
   }
   HEAP32[$2 + 8 >> 2] = $1;
   HEAP32[$2 + 4 >> 2] = $0;
   HEAP32[$2 >> 2] = 1;
   printk(30726, $2);
   $0 = -38;
  }
  global$0 = $2 + 16 | 0;
  return $0 | 0;
 }
 
 function __syscall2($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $4 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($4 | 0) != 96) {
    $0 = FUNCTION_TABLE[$4]($1, $2) | 0;
    break label$1;
   }
   HEAP32[$3 + 12 >> 2] = $2;
   HEAP32[$3 + 8 >> 2] = $1;
   HEAP32[$3 + 4 >> 2] = $0;
   HEAP32[$3 >> 2] = 2;
   printk(30755, $3);
   $0 = -38;
  }
  global$0 = $3 + 16 | 0;
  return $0 | 0;
 }
 
 function __syscall3($0, $1, $2, $3) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  var $4 = 0, $5 = 0;
  $4 = global$0 - 32 | 0;
  global$0 = $4;
  $5 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($5 | 0) != 96) {
    $0 = FUNCTION_TABLE[$5]($1, $2, $3) | 0;
    break label$1;
   }
   HEAP32[$4 + 16 >> 2] = $3;
   HEAP32[$4 + 12 >> 2] = $2;
   HEAP32[$4 + 8 >> 2] = $1;
   HEAP32[$4 + 4 >> 2] = $0;
   HEAP32[$4 >> 2] = 3;
   printk(30788, $4);
   $0 = -38;
  }
  global$0 = $4 + 32 | 0;
  return $0 | 0;
 }
 
 function __syscall4($0, $1, $2, $3, $4) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  $4 = $4 | 0;
  var $5 = 0, $6 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  $6 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($6 | 0) != 96) {
    $0 = FUNCTION_TABLE[$6]($1, $2, $3, $4) | 0;
    break label$1;
   }
   HEAP32[$5 + 20 >> 2] = $4;
   HEAP32[$5 + 16 >> 2] = $3;
   HEAP32[$5 + 12 >> 2] = $2;
   HEAP32[$5 + 8 >> 2] = $1;
   HEAP32[$5 + 4 >> 2] = $0;
   HEAP32[$5 >> 2] = 4;
   printk(30825, $5);
   $0 = -38;
  }
  global$0 = $5 + 32 | 0;
  return $0 | 0;
 }
 
 function __syscall5($0, $1, $2, $3, $4, $5) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  $4 = $4 | 0;
  $5 = $5 | 0;
  var $6 = 0, $7 = 0;
  $6 = global$0 - 32 | 0;
  global$0 = $6;
  $7 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($7 | 0) != 96) {
    $0 = FUNCTION_TABLE[$7]($1, $2, $3, $4, $5) | 0;
    break label$1;
   }
   HEAP32[$6 + 24 >> 2] = $5;
   HEAP32[$6 + 20 >> 2] = $4;
   HEAP32[$6 + 16 >> 2] = $3;
   HEAP32[$6 + 12 >> 2] = $2;
   HEAP32[$6 + 8 >> 2] = $1;
   HEAP32[$6 + 4 >> 2] = $0;
   HEAP32[$6 >> 2] = 5;
   printk(30866, $6);
   $0 = -38;
  }
  global$0 = $6 + 32 | 0;
  return $0 | 0;
 }
 
 function __syscall6($0, $1, $2, $3, $4, $5, $6) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $3 = $3 | 0;
  $4 = $4 | 0;
  $5 = $5 | 0;
  $6 = $6 | 0;
  var $7 = 0, $8 = 0;
  $7 = global$0 - 32 | 0;
  global$0 = $7;
  $8 = HEAP32[($0 << 2) + 29520 >> 2];
  label$1 : {
   if (($8 | 0) != 96) {
    $0 = FUNCTION_TABLE[$8]($1, $2, $3, $4, $5, $6) | 0;
    break label$1;
   }
   HEAP32[$7 + 28 >> 2] = $6;
   HEAP32[$7 + 24 >> 2] = $5;
   HEAP32[$7 + 20 >> 2] = $4;
   HEAP32[$7 + 16 >> 2] = $3;
   HEAP32[$7 + 12 >> 2] = $2;
   HEAP32[$7 + 8 >> 2] = $1;
   HEAP32[$7 + 4 >> 2] = $0;
   HEAP32[$7 >> 2] = 6;
   printk(30911, $7);
   $0 = -38;
  }
  global$0 = $7 + 32 | 0;
  return $0 | 0;
 }
 
 function setup_processor() {
  var $0 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 31052;
  printk(31022, $0);
  arc_init_IRQ();
  arc_mmu_init();
  arc_cache_init();
  global$0 = $0 + 16 | 0;
 }
 
 function machine_restart() {
  var $0 = 0;
  $0 = global$0 - 32 | 0;
  global$0 = $0;
  HEAP32[$0 + 16 >> 2] = 31132;
  printk(31102, $0 + 16 | 0);
  HEAP32[$0 >> 2] = 31089;
  printk(31068, $0);
  global$0 = $0 + 32 | 0;
 }
 
 function arc_init_IRQ() {
  var $0 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 31169;
  printk(31148, $0);
  global$0 = $0 + 16 | 0;
 }
 
 function setup_arch_memory() {
  var $0 = 0, $1 = 0, $2 = 0;
  $0 = global$0 - 48 | 0;
  global$0 = $0;
  HEAP32[3967] = 19448;
  HEAP32[3965] = 19436;
  HEAP32[3963] = 19428;
  HEAP32[3962] = 82964;
  HEAP32[$0 + 16 >> 2] = 19448;
  HEAP32[$0 + 12 >> 2] = 19436;
  HEAP32[$0 + 8 >> 2] = 19428;
  HEAP32[$0 + 4 >> 2] = 82964;
  HEAP32[$0 >> 2] = 31310;
  printk(31208, $0);
  $2 = HEAP32[22036];
  $1 = $2 + 65535 >>> 16 | 0;
  HEAP32[20637] = $1;
  HEAP32[20635] = $1;
  HEAP32[20644] = $1;
  HEAP32[20636] = 0;
  memblock_add_range(15692, 65535, $2);
  memblock_reserve(65535, -46087);
  if (HEAP32[20633]) {
   __memblock_dump_all()
  }
  memset($0 + 40 | 0, 0, 8);
  memset($0 + 32 | 0, 0, 8);
  HEAP32[$0 + 32 >> 2] = 0;
  $1 = HEAP32[20636];
  HEAP32[$0 + 40 >> 2] = HEAP32[20635] - $1;
  HEAP32[35583] = $1;
  HEAP32[35586] = 0;
  HEAP32[35613] = 0;
  calculate_node_totalpages($1, $0 + 40 | 0, $0 + 32 | 0);
  free_area_init_core();
  global$0 = $0 + 48 | 0;
 }
 
 function mem_init() {
  var $0 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 31344;
  printk(31328, $0);
  reset_all_zones_managed_pages();
  (wasm2js_i32$0 = 82820, wasm2js_i32$1 = free_low_memory_core_early() + HEAP32[20705] | 0), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  mem_init_print_info();
  global$0 = $0 + 16 | 0;
 }
 
 function arc_cache_mumbojumbo($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $1 = global$0 - 96 | 0;
  global$0 = $1;
  $2 = HEAP32[20720];
  $3 = $2 >>> 14 & 255;
  label$1 : {
   if ($3) {
    HEAP32[$1 + 84 >> 2] = 31435;
    HEAP32[$1 + 80 >> 2] = $2 & 67108864 ? 31424 : 31434;
    HEAP32[$1 + 72 >> 2] = $3;
    HEAP32[$1 + 64 >> 2] = $2 & 16383;
    HEAP32[$1 + 76 >> 2] = $2 & 134217728 ? 31414 : 31419;
    HEAP32[$1 + 68 >> 2] = $2 >>> 22 & 15;
    $3 = scnprintf($0, 256, 31369, $1 - -64 | 0);
    break label$1;
   }
   $3 = scnprintf($0, 256, 31353, 0);
  }
  $4 = 256 - $3 | 0;
  $5 = $0 + $3 | 0;
  $2 = HEAP32[20721];
  $6 = $2 >>> 14 & 255;
  label$3 : {
   if ($6) {
    HEAP32[$1 + 52 >> 2] = 31435;
    HEAP32[$1 + 48 >> 2] = $2 & 67108864 ? 31424 : 31434;
    HEAP32[$1 + 40 >> 2] = $6;
    HEAP32[$1 + 32 >> 2] = $2 & 16383;
    HEAP32[$1 + 44 >> 2] = $2 & 134217728 ? 31414 : 31419;
    HEAP32[$1 + 36 >> 2] = $2 >>> 22 & 15;
    $2 = scnprintf($5, $4, 31463, $1 + 32 | 0);
    break label$3;
   }
   $2 = scnprintf($5, $4, 31447, 0);
  }
  $2 = $2 + $3 | 0;
  $3 = HEAP32[20722];
  $4 = $3 >>> 14 & 255;
  if ($4) {
   HEAP32[$1 + 20 >> 2] = $4;
   HEAP32[$1 + 16 >> 2] = $3 & 16383;
   HEAP32[$1 + 24 >> 2] = HEAP32[4940] ? 31434 : 31435;
   $2 = scnprintf($0 + $2 | 0, 256 - $2 | 0, 31508, $1 + 16 | 0) + $2 | 0;
  }
  HEAP32[$1 + 8 >> 2] = 31434;
  HEAP32[$1 + 4 >> 2] = 31434;
  HEAP32[$1 >> 2] = HEAP32[4941];
  scnprintf($0 + $2 | 0, 256 - $2 | 0, 31532, $1);
  global$0 = $1 + 96 | 0;
  return $0;
 }
 
 function arc_cache_init() {
  var $0 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $0 = global$0 - 272 | 0;
  global$0 = $0;
  (wasm2js_i32$0 = $0, wasm2js_i32$1 = arc_cache_mumbojumbo($0 + 16 | 0)), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
  printk(31556, $0);
  HEAP32[34817] = 100;
  HEAP32[34816] = 101;
  HEAP32[34818] = 102;
  global$0 = $0 + 272 | 0;
 }
 
 function utlb_invalidate() {
  var $0 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 31616;
  panic(31594, $0);
  abort();
 }
 
 function local_flush_tlb_page($0) {
  if (!HEAP32[HEAP32[$0 + 32 >> 2] + 360 >> 2]) {
   return
  }
  utlb_invalidate();
  abort();
 }
 
 function arc_mmu_init() {
  var $0 = 0;
  $0 = global$0 - 16 | 0;
  global$0 = $0;
  HEAP32[$0 >> 2] = 31581;
  printk(31561, $0);
  global$0 = $0 + 16 | 0;
 }
 
 function __dynamic_pr_debug($0, $1, $2) {
  var $3 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $3 = global$0 - 128 | 0;
  global$0 = $3;
  label$1 : {
   if ($0) {
    if (!$1) {
     break label$1
    }
    HEAP32[$3 + 124 >> 2] = $2;
    HEAP32[$3 + 112 >> 2] = $1;
    HEAP32[$3 + 116 >> 2] = $3 + 124;
    (wasm2js_i32$0 = $3, wasm2js_i32$1 = dynamic_emit_prefix($0, $3 + 48 | 0)), HEAP32[wasm2js_i32$0 + 32 >> 2] = wasm2js_i32$1;
    HEAP32[$3 + 36 >> 2] = $3 + 112;
    printk(31708, $3 + 32 | 0);
    global$0 = $3 + 128 | 0;
    return;
   }
   HEAP32[$3 + 12 >> 2] = 31673;
   HEAP32[$3 + 8 >> 2] = 557;
   HEAP32[$3 + 4 >> 2] = 31692;
   HEAP32[$3 >> 2] = 31673;
   printk(31632, $3);
   abort();
  }
  HEAP32[$3 + 28 >> 2] = 31673;
  HEAP32[$3 + 24 >> 2] = 558;
  HEAP32[$3 + 20 >> 2] = 31692;
  HEAP32[$3 + 16 >> 2] = 31673;
  printk(31632, $3 + 16 | 0);
  abort();
 }
 
 function dynamic_emit_prefix($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $3 = global$0 + -64 | 0;
  global$0 = $3;
  HEAP8[$1 | 0] = 0;
  $4 = 0;
  label$1 : {
   if (!(HEAPU8[$0 + 18 | 0] & 64)) {
    break label$1
   }
   if (HEAP32[1] & 2096896) {
    $4 = snprintf($1, 64, 31716, 0);
    break label$1;
   }
   (wasm2js_i32$0 = $3, wasm2js_i32$1 = __task_pid_nr_ns(HEAP32[2], 0, 0)), HEAP32[wasm2js_i32$0 + 48 >> 2] = wasm2js_i32$1;
   $4 = snprintf($1, 64, 31724, $3 + 48 | 0);
  }
  $2 = $4;
  $6 = $0 + 16 | 0;
  $5 = HEAP32[$6 >> 2];
  if ($5 & 524288) {
   HEAP32[$3 + 32 >> 2] = HEAP32[$0 >> 2];
   $2 = 64 - $4 | 0;
   $2 = snprintf($1 + $4 | 0, ($2 | 0) > 0 ? $2 : 0, 31730, $3 + 32 | 0) + $4 | 0;
   $5 = HEAP32[$6 >> 2];
  }
  if ($5 & 1048576) {
   HEAP32[$3 + 16 >> 2] = HEAP32[$0 + 4 >> 2];
   $5 = $2;
   $6 = $1 + $2 | 0;
   $2 = 64 - $2 | 0;
   $2 = $5 + snprintf($6, ($2 | 0) > 0 ? $2 : 0, 31730, $3 + 16 | 0) | 0;
   $5 = HEAP32[$0 + 16 >> 2];
  }
  if ($5 & 2097152) {
   HEAP32[$3 >> 2] = $5 & 262143;
   $0 = 64 - $2 | 0;
   $2 = snprintf($1 + $2 | 0, ($0 | 0) > 0 ? $0 : 0, 31734, $3) + $2 | 0;
  }
  if (($2 | 0) != ($4 | 0)) {
   $0 = 64 - $2 | 0;
   $4 = snprintf($1 + $2 | 0, ($0 | 0) > 0 ? $0 : 0, 31738, 0) + $2 | 0;
  }
  if (($4 | 0) >= 64) {
   HEAP8[$1 + 63 | 0] = 0
  }
  global$0 = $3 - -64 | 0;
  return $1;
 }
 
 function hex_dump_to_buffer($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $5 = global$0 - 48 | 0;
  global$0 = $5;
  $8 = 32 < $1 >>> 0 ? 32 : $1;
  $1 = $8 & 3 ? 1 : 4;
  $7 = ($8 >>> 0) / ($1 >>> 0) | 0;
  label$2 : {
   label$4 : {
    label$5 : {
     if (!$8) {
      break label$5
     }
     label$7 : {
      label$10 : {
       if (($1 | 0) != 2) {
        if (($1 | 0) == 4) {
         break label$10
        }
        if (($1 | 0) != 8) {
         break label$7
        }
        if (($7 | 0) < 1) {
         break label$5
        }
        $1 = $0;
        while (1) {
         $0 = HEAPU8[$1 + 4 | 0] | HEAPU8[$1 + 5 | 0] << 8 | (HEAPU8[$1 + 6 | 0] << 16 | HEAPU8[$1 + 7 | 0] << 24);
         HEAP32[$5 + 8 >> 2] = HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8 | (HEAPU8[$1 + 2 | 0] << 16 | HEAPU8[$1 + 3 | 0] << 24);
         HEAP32[$5 + 12 >> 2] = $0;
         HEAP32[$5 >> 2] = $3 ? 31805 : 31807;
         $0 = 131 - $4 | 0;
         $6 = snprintf($2 + $4 | 0, $0, 31793, $5);
         if ($6 >>> 0 >= $0 >>> 0) {
          break label$2
         }
         $1 = $1 + 8 | 0;
         $4 = $4 + $6 | 0;
         $3 = $3 + 1 | 0;
         if (($3 | 0) < ($7 | 0)) {
          continue
         }
         break;
        };
        break label$5;
       }
       if (($7 | 0) < 1) {
        break label$5
       }
       $1 = $0;
       while (1) {
        HEAP32[$5 + 36 >> 2] = HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8;
        HEAP32[$5 + 32 >> 2] = $3 ? 31805 : 31807;
        $0 = 131 - $4 | 0;
        $6 = snprintf($2 + $4 | 0, $0, 31816, $5 + 32 | 0);
        if ($6 >>> 0 >= $0 >>> 0) {
         break label$2
        }
        $1 = $1 + 2 | 0;
        $4 = $4 + $6 | 0;
        $3 = $3 + 1 | 0;
        if (($3 | 0) < ($7 | 0)) {
         continue
        }
        break;
       };
       break label$5;
      }
      if (($7 | 0) < 1) {
       break label$5
      }
      $1 = $0;
      while (1) {
       HEAP32[$5 + 20 >> 2] = HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8 | (HEAPU8[$1 + 2 | 0] << 16 | HEAPU8[$1 + 3 | 0] << 24);
       HEAP32[$5 + 16 >> 2] = $3 ? 31805 : 31807;
       $0 = 131 - $4 | 0;
       $6 = snprintf($2 + $4 | 0, $0, 31808, $5 + 16 | 0);
       if ($6 >>> 0 >= $0 >>> 0) {
        break label$2
       }
       $1 = $1 + 4 | 0;
       $4 = $4 + $6 | 0;
       $3 = $3 + 1 | 0;
       if (($3 | 0) < ($7 | 0)) {
        continue
       }
       break;
      };
      break label$5;
     }
     $1 = 0;
     label$15 : {
      label$16 : {
       while (1) {
        $4 = $3 + 2 | 0;
        if ($4 >>> 0 > 131) {
         break label$16
        }
        $7 = $2 + $3 | 0;
        $9 = HEAPU8[$0 + $1 | 0];
        HEAP8[$7 | 0] = HEAPU8[($9 >>> 4 | 0) + 31744 | 0];
        $6 = $3 + 3 | 0;
        if ($6 >>> 0 > 131) {
         break label$15
        }
        HEAP8[$7 + 1 | 0] = HEAPU8[($9 & 15) + 31744 | 0];
        if ($3 + 4 >>> 0 > 131) {
         break label$4
        }
        HEAP8[$7 + 2 | 0] = 32;
        $3 = $6;
        $1 = $1 + 1 | 0;
        if ($1 >>> 0 < $8 >>> 0) {
         continue
        }
        break;
       };
       $4 = $3 + -1 | 0;
       break label$5;
      }
      $4 = $3;
      break label$4;
     }
     $4 = $3 + 1 | 0;
     break label$4;
    }
    HEAP8[$2 + $4 | 0] = 0;
    break label$2;
   }
   HEAP8[$2 + $4 | 0] = 0;
  }
  global$0 = $5 + 48 | 0;
 }
 
 function print_hex_dump($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = global$0 - 192 | 0;
  global$0 = $1;
  $2 = 36;
  while (1) {
   hex_dump_to_buffer($0 + $3 | 0, ($2 | 0) < 32 ? $2 : 32, $1 + 48 | 0);
   HEAP32[$1 + 4 >> 2] = 26125;
   HEAP32[$1 >> 2] = 26122;
   HEAP32[$1 + 8 >> 2] = $1 + 48;
   printk(31850, $1);
   $2 = $2 - 32 | 0;
   $3 = $3 + 32 | 0;
   if ($3 >>> 0 < 36) {
    continue
   }
   break;
  };
  global$0 = $1 + 192 | 0;
 }
 
 function string_escape_mem($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  if (!$1) {
   return 0
  }
  $6 = $2 + $3 | 0;
  $7 = $4 & 32;
  $8 = $4 & 8;
  $9 = $4 & 4;
  $10 = $4 & 2;
  $11 = $4 & 1;
  $12 = $4 & 16;
  $4 = $2;
  while (1) {
   $1 = $1 + -1 | 0;
   label$4 : {
    label$6 : {
     label$7 : {
      label$8 : {
       label$9 : {
        label$10 : {
         label$11 : {
          $3 = HEAPU8[$0 | 0];
          if (!(HEAPU8[$3 + 31952 | 0] & 151 ? $12 : 0)) {
           label$15 : {
            if (!$11) {
             break label$15
            }
            $5 = $3 + -9 | 0;
            if (($5 & 255) >>> 0 > 4) {
             break label$15
            }
            if ($6 >>> 0 > $4 >>> 0) {
             HEAP8[$4 | 0] = 92
            }
            $3 = $4 + 1 | 0;
            if ($3 >>> 0 < $6 >>> 0) {
             $13 = $3;
             $5 = ($5 & 255) << 3;
             $3 = $5 & 31;
             if (32 <= ($5 & 63) >>> 0) {
              $3 = 114 >>> $3 | 0
             } else {
              $3 = ((1 << $3) - 1 & 114) << 32 - $3 | 1719037556 >>> $3
             }
             HEAP8[$13 | 0] = $3;
            }
            $4 = $4 + 2 | 0;
            break label$4;
           }
           label$18 : {
            if (!$10) {
             break label$18
            }
            if (($3 | 0) != 92) {
             if (($3 | 0) == 27) {
              break label$10
             }
             if (($3 | 0) != 7) {
              break label$18
             }
             $3 = 97;
            }
            if ($6 >>> 0 <= $4 >>> 0) {
             break label$6
            }
            break label$7;
           }
           if ($3 ? 0 : $9) {
            break label$8
           }
           if ($8) {
            break label$11
           }
           if ($7) {
            break label$9
           }
          }
          if ($6 >>> 0 > $4 >>> 0) {
           HEAP8[$4 | 0] = $3
          }
          $4 = $4 + 1 | 0;
          break label$4;
         }
         if ($6 >>> 0 > $4 >>> 0) {
          HEAP8[$4 | 0] = 92
         }
         $5 = $4 + 1 | 0;
         if ($5 >>> 0 < $6 >>> 0) {
          HEAP8[$5 | 0] = $3 >>> 6 | 48
         }
         $5 = $4 + 2 | 0;
         if ($5 >>> 0 < $6 >>> 0) {
          HEAP8[$5 | 0] = $3 >>> 3 & 7 | 48
         }
         $5 = $4 + 3 | 0;
         if ($5 >>> 0 < $6 >>> 0) {
          HEAP8[$5 | 0] = $3 & 7 | 48
         }
         $4 = $4 + 4 | 0;
         break label$4;
        }
        $3 = 101;
        if ($6 >>> 0 > $4 >>> 0) {
         break label$7
        }
        break label$6;
       }
       if ($6 >>> 0 > $4 >>> 0) {
        HEAP8[$4 | 0] = 92
       }
       $5 = $4 + 1 | 0;
       if ($5 >>> 0 < $6 >>> 0) {
        HEAP8[$5 | 0] = 120
       }
       $5 = $4 + 2 | 0;
       if ($5 >>> 0 < $6 >>> 0) {
        HEAP8[$5 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0]
       }
       $5 = $4 + 3 | 0;
       if ($5 >>> 0 < $6 >>> 0) {
        HEAP8[$5 | 0] = HEAPU8[($3 & 15) + 31744 | 0]
       }
       $4 = $4 + 4 | 0;
       break label$4;
      }
      if ($6 >>> 0 > $4 >>> 0) {
       HEAP8[$4 | 0] = 92
      }
      $3 = $4 + 1 | 0;
      if ($3 >>> 0 < $6 >>> 0) {
       HEAP8[$3 | 0] = 48
      }
      $4 = $4 + 2 | 0;
      break label$4;
     }
     HEAP8[$4 | 0] = 92;
    }
    $5 = $4 + 1 | 0;
    if ($5 >>> 0 < $6 >>> 0) {
     HEAP8[$5 | 0] = $3
    }
    $4 = $4 + 2 | 0;
   }
   $0 = $0 + 1 | 0;
   if ($1) {
    continue
   }
   break;
  };
  return $4 - $2 | 0;
 }
 
 function errseq_set($0, $1) {
  var $2 = 0;
  $2 = HEAP32[$0 >> 2];
  label$1 : {
   if (!$1) {
    break label$1
   }
   $1 = 0 - $1 | 0;
   if ($1 >>> 0 > 4095) {
    break label$1
   }
   $1 = $1 | $2 & -8192;
   $1 = $2 & 4096 ? $1 - -8192 | 0 : $1;
   if (($1 | 0) == ($2 | 0)) {
    break label$1
   }
   HEAP32[$0 >> 2] = $1;
  }
 }
 
 function refcount_dec_and_lock_irqsave($0, $1) {
  var $2 = 0, $3 = 0;
  $3 = 1;
  $2 = HEAP32[$0 >> 2];
  label$1 : {
   label$2 : {
    if (($2 | 0) != 1) {
     if ($2 + 1 >>> 0 < 2) {
      break label$2
     }
     HEAP32[$0 >> 2] = $2 + -1;
     break label$2;
    }
    $2 = HEAP32[34819];
    HEAP32[34819] = 0;
    HEAP32[$1 >> 2] = $2;
    $2 = $0;
    $0 = HEAP32[$0 >> 2] + -1 | 0;
    HEAP32[$2 >> 2] = $0;
    if (!$0) {
     break label$1
    }
    HEAP32[34819] = HEAP32[$1 >> 2];
   }
   $3 = 0;
  }
  return $3;
 }
 
 function find_next_bit($0, $1, $2) {
  var $3 = 0, $4 = 0;
  label$1 : {
   if ($2 >>> 0 >= $1 >>> 0) {
    break label$1
   }
   $3 = $2 & -32;
   $2 = HEAP32[($2 >>> 3 & 536870908) + $0 >> 2] & -1 << ($2 & 31);
   if (!$2) {
    while (1) {
     $3 = $3 + 32 | 0;
     if ($3 >>> 0 >= $1 >>> 0) {
      break label$1
     }
     $2 = HEAP32[($3 >>> 3 | 0) + $0 >> 2];
     if (!$2) {
      continue
     }
     break;
    }
   }
   $4 = $3;
   $0 = $2 & 65535;
   $3 = !$0 << 4;
   $0 = $0 ? $2 : $2 >>> 16 | 0;
   $2 = $0 & 255;
   $3 = $2 ? $3 : $3 | 8;
   $0 = $2 ? $0 : $0 >>> 8 | 0;
   $2 = $0 & 15;
   $3 = $2 ? $3 : $3 | 4;
   $0 = $2 ? $0 : $0 >>> 4 | 0;
   $2 = $0 & 3;
   $0 = ($4 + ($2 ? $3 : $3 | 2) | 0) + ((($2 ? $0 : $0 >>> 2 | 0) ^ -1) & 1) | 0;
   $1 = $0 >>> 0 < $1 >>> 0 ? $0 : $1;
  }
  return $1;
 }
 
 function __bitmap_clear($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = -1 << ($1 & 31);
  $0 = ($1 >>> 3 & 536870908) + $0 | 0;
  $4 = ($1 | -32) + $2 | 0;
  label$1 : {
   label$2 : {
    if (($4 | 0) >= 0) {
     HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & ($3 ^ -1);
     $0 = $0 + 4 | 0;
     $3 = $4 + -32 | 0;
     if (($3 | 0) >= 0) {
      while (1) {
       HEAP32[$0 >> 2] = 0;
       $0 = $0 + 4 | 0;
       $3 = $3 + -32 | 0;
       if (($3 | 0) > -1) {
        continue
       }
       break;
      }
     }
     $3 = -1;
     if (!($4 & 31)) {
      break label$1
     }
     break label$2;
    }
    if (!$2) {
     break label$1
    }
   }
   HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & (-1 >>> (0 - ($1 + $2 | 0) & 31) & $3 ^ -1);
  }
 }
 
 function bust_spinlocks($0) {
  if ($0) {
   HEAP32[19560] = HEAP32[19560] + 1;
   return;
  }
  console_unblank();
  $0 = HEAP32[19560] + -1 | 0;
  HEAP32[19560] = $0;
  if ($0) {
   return
  }
  if (HEAP32[2972] != 11888) {
   HEAP32[2979] = HEAP32[2979] | 1;
   irq_work_queue(11920);
  }
 }
 
 function debug_locks_off() {
  var $0 = 0;
  label$1 : {
   if (!HEAP32[4943]) {
    break label$1
   }
   $0 = HEAP32[4943];
   HEAP32[4943] = 0;
   if (!$0) {
    break label$1
   }
   if (HEAP32[34820]) {
    break label$1
   }
   if (!HEAP32[2968]) {
    break label$1
   }
   HEAP32[2968] = 15;
  }
 }
 
 function u32_swap($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  $2 = HEAP32[$0 >> 2];
  HEAP32[$0 >> 2] = HEAP32[$1 >> 2];
  HEAP32[$1 >> 2] = $2;
 }
 
 function generic_swap($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0;
  while (1) {
   $3 = HEAPU8[$0 | 0];
   HEAP8[$0 | 0] = HEAPU8[$1 | 0];
   HEAP8[$1 | 0] = $3;
   $1 = $1 + 1 | 0;
   $0 = $0 + 1 | 0;
   $2 = $2 + -1 | 0;
   if (($2 | 0) > 0) {
    continue
   }
   break;
  };
 }
 
 function u64_swap($0, $1, $2) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $2 = $2 | 0;
  var $3 = 0, $4 = 0;
  $2 = HEAP32[$0 >> 2];
  $3 = HEAP32[$0 + 4 >> 2];
  $4 = HEAP32[$1 + 4 >> 2];
  HEAP32[$0 >> 2] = HEAP32[$1 >> 2];
  HEAP32[$0 + 4 >> 2] = $4;
  HEAP32[$1 >> 2] = $2;
  HEAP32[$1 + 4 >> 2] = $3;
 }
 
 function __div64_32($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $3 = HEAP32[$0 + 4 >> 2];
  $5 = $3;
  $6 = HEAP32[$0 >> 2];
  if ($3 >>> 0 >= $1 >>> 0) {
   $2 = ($3 >>> 0) / ($1 >>> 0) | 0;
   $9 = $2;
   $2 = Math_imul($1, $2);
   $3 = $6;
   $6 = $3 - 0 | 0;
   $5 = $5 - (($3 >>> 0 < 0) + $2 | 0) | 0;
  }
  $4 = $1;
  $1 = 0;
  label$2 : {
   label$3 : {
    if (!$4) {
     break label$3
    }
    if (!$5 & $6 >>> 0 <= $4 >>> 0 | $5 >>> 0 < 0) {
     break label$3
    }
    $7 = 1;
    $3 = 0;
    while (1) {
     $2 = $3 << 1 | $7 >>> 31;
     $7 = $7 << 1;
     $3 = $2;
     $2 = $1 << 1 | $4 >>> 31;
     $4 = $4 << 1;
     $8 = $4;
     $1 = $2;
     if (($1 | 0) < 0) {
      $2 = 1
     } else {
      if (($1 | 0) <= 0) {
       $2 = $8 >>> 0 >= 1 ? 0 : 1
      } else {
       $2 = 0
      }
     }
     if ($2) {
      break label$2
     }
     if (($1 | 0) == ($5 | 0) & $4 >>> 0 < $6 >>> 0 | $1 >>> 0 < $5 >>> 0) {
      continue
     }
     break;
    };
    break label$2;
   }
   $7 = 1;
   $3 = 0;
  }
  while (1) {
   $8 = ($1 | 0) == ($5 | 0) & $6 >>> 0 < $4 >>> 0 | $5 >>> 0 < $1 >>> 0;
   $2 = $10 + ($8 ? 0 : $7) | 0;
   $9 = $9 + ($8 ? 0 : $3) | 0;
   $9 = $2 >>> 0 < $10 >>> 0 ? $9 + 1 | 0 : $9;
   $10 = $2;
   $2 = $6;
   $11 = $8 ? 0 : $4;
   $6 = $2 - $11 | 0;
   $5 = $5 - (($2 >>> 0 < $11 >>> 0) + ($8 ? 0 : $1) | 0) | 0;
   $4 = ($1 & 1) << 31 | $4 >>> 1;
   $1 = $1 >>> 1 | 0;
   $7 = ($3 & 1) << 31 | $7 >>> 1;
   $3 = $3 >>> 1 | 0;
   if ($7 | $3) {
    continue
   }
   break;
  };
  HEAP32[$0 >> 2] = $10;
  HEAP32[$0 + 4 >> 2] = $9;
  return $6;
 }
 
 function div_s64_rem($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      if (($1 | 0) < -1) {
       $4 = 1
      } else {
       if (($1 | 0) <= -1) {
        $4 = $0 >>> 0 > 4294967295 ? 0 : 1
       } else {
        $4 = 0
       }
      }
      if (!$4) {
       $4 = $2 >> 31;
       $6 = $4 + $2 ^ $4;
       HEAP32[$5 + 8 >> 2] = $0;
       HEAP32[$5 + 12 >> 2] = $1;
       if (!$1 & $0 >>> 0 > 4294967295 | $1 >>> 0 > 0) {
        break label$4
       }
       $1 = 0;
       $4 = $0;
       $0 = ($0 >>> 0) / ($6 >>> 0) | 0;
       HEAP32[$5 + 8 >> 2] = $0;
       HEAP32[$5 + 12 >> 2] = 0;
       $4 = $4 - Math_imul($6, $0) | 0;
       break label$1;
      }
      $4 = 0 - $0 | 0;
      HEAP32[$5 + 8 >> 2] = $4;
      $1 = 0 - ((0 < $0 >>> 0) + $1 | 0) | 0;
      HEAP32[$5 + 12 >> 2] = $1;
      $0 = $2 >> 31;
      $6 = $0 + $2 ^ $0;
      if (!$1 & $4 >>> 0 > 4294967295 | $1 >>> 0 > 0) {
       break label$3
      }
      $0 = ($4 >>> 0) / ($6 >>> 0) | 0;
      HEAP32[$5 + 8 >> 2] = $0;
      HEAP32[$5 + 12 >> 2] = 0;
      $4 = $4 - Math_imul($6, $0) | 0;
      $1 = 0;
      break label$2;
     }
     $4 = __div64_32($5 + 8 | 0, $6);
     $0 = HEAP32[$5 + 8 >> 2];
     $1 = HEAP32[$5 + 12 >> 2];
     break label$1;
    }
    $4 = __div64_32($5 + 8 | 0, $6);
    $0 = HEAP32[$5 + 8 >> 2];
    $1 = HEAP32[$5 + 12 >> 2];
   }
   HEAP32[$3 >> 2] = 0 - $4;
   global$0 = $5 + 16 | 0;
   $3 = ($2 | 0) > 0;
   $2 = $3 ? 0 - $0 | 0 : $0;
   i64toi32_i32$HIGH_BITS = $3 ? 0 - ((0 < $0 >>> 0) + $1 | 0) | 0 : $1;
   return $2;
  }
  HEAP32[$3 >> 2] = $4;
  global$0 = $5 + 16 | 0;
  $3 = ($2 | 0) < 0;
  $2 = $3 ? 0 - $0 | 0 : $0;
  i64toi32_i32$HIGH_BITS = $3 ? 0 - ((0 < $0 >>> 0) + $1 | 0) | 0 : $1;
  return $2;
 }
 
 function div64_u64($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      $4 = $3;
      if ($4) {
       $8 = $0;
       $5 = $4 >>> 0 > 65535;
       $6 = $5 ? 32 : 16;
       $4 = $5 ? $4 : $4 << 16;
       $5 = $4 >>> 0 > 16777215;
       $6 = $5 ? $6 : $6 + -8 | 0;
       $4 = $5 ? $4 : $4 << 8;
       $5 = $4 >>> 0 > 268435455;
       $6 = $5 ? $6 : $6 + -4 | 0;
       $4 = $5 ? $4 : $4 << 4;
       $5 = $4 >>> 0 > 1073741823;
       $6 = ($5 ? $6 : $6 + -2 | 0) - (($5 ? $4 : $4 << 2) >> 31) | 0;
       $4 = $6;
       $5 = $4 & 31;
       $9 = $7;
       if (32 <= ($4 & 63) >>> 0) {
        $4 = 0;
        $8 = $1 >>> $5 | 0;
       } else {
        $4 = $1 >>> $5 | 0;
        $8 = ((1 << $5) - 1 & $1) << 32 - $5 | $8 >>> $5;
       }
       HEAP32[$9 + 8 >> 2] = $8;
       HEAP32[$7 + 12 >> 2] = $4;
       $5 = $3;
       $9 = $2;
       $3 = $6 & 31;
       if (32 <= ($6 & 63) >>> 0) {
        $6 = $5 >>> $3 | 0
       } else {
        $6 = ((1 << $3) - 1 & $5) << 32 - $3 | $9 >>> $3
       }
       if (!$4 & $8 >>> 0 > 4294967295 | $4 >>> 0 > 0) {
        break label$4
       }
       $3 = 0;
       $4 = ($8 >>> 0) / ($6 >>> 0) | 0;
       HEAP32[$7 + 8 >> 2] = $4;
       HEAP32[$7 + 12 >> 2] = 0;
       break label$2;
      }
      HEAP32[$7 + 8 >> 2] = $0;
      HEAP32[$7 + 12 >> 2] = $1;
      if (!$1 & $0 >>> 0 > 4294967295 | $1 >>> 0 > 0) {
       break label$3
      }
      $3 = 0;
      $2 = ($0 >>> 0) / ($2 >>> 0) | 0;
      HEAP32[$7 + 8 >> 2] = $2;
      HEAP32[$7 + 12 >> 2] = 0;
      break label$1;
     }
     __div64_32($7 + 8 | 0, $6);
     $4 = HEAP32[$7 + 8 >> 2];
     $3 = HEAP32[$7 + 12 >> 2];
     break label$2;
    }
    __div64_32($7 + 8 | 0, $2);
    $2 = HEAP32[$7 + 8 >> 2];
    $3 = HEAP32[$7 + 12 >> 2];
    break label$1;
   }
   $9 = $0;
   $6 = $3 + -1 | 0;
   $8 = $4 + -1 | 0;
   if ($8 >>> 0 < 4294967295) {
    $6 = $6 + 1 | 0
   }
   $3 = !($3 | $4);
   $4 = $3 ? 0 : $8;
   $3 = $3 ? 0 : $6;
   $6 = __wasm_i64_mul($4, $3, $2, $5);
   $8 = $9 - $6 | 0;
   $0 = $1 - (i64toi32_i32$HIGH_BITS + ($0 >>> 0 < $6 >>> 0) | 0) | 0;
   $0 = ($5 | 0) == ($0 | 0) & $8 >>> 0 >= $2 >>> 0 | $0 >>> 0 > $5 >>> 0;
   $1 = $0 + $4 | 0;
   if ($1 >>> 0 < $0 >>> 0) {
    $3 = $3 + 1 | 0
   }
   $2 = $1;
  }
  global$0 = $7 + 16 | 0;
  i64toi32_i32$HIGH_BITS = $3;
  return $2;
 }
 
 function add_device_randomness($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  HEAP32[$4 + 12 >> 2] = HEAP32[20745];
  label$1 : {
   if (!$1) {
    break label$1
   }
   if (HEAP32[34838] > 1) {
    break label$1
   }
   if (HEAP32[34838]) {
    break label$1
   }
   $7 = $1 >>> 0 > 32 ? $1 : 32;
   if ($7) {
    $2 = HEAPU8[19836];
    while (1) {
     $3 = ($5 & 31) + 139300 | 0;
     $6 = HEAPU8[$3 | 0];
     $8 = $3;
     $3 = $2 >>> 1 & 127;
     $2 = $2 & 1 ? $3 ^ -31 : $3;
     HEAP8[$8 | 0] = HEAPU8[(($5 >>> 0) % ($1 >>> 0) | 0) + $0 | 0] ^ ($6 ^ $2);
     $2 = $2 + ($6 << 3 | $6 >>> 5) | 0;
     $5 = $5 + 1 | 0;
     if ($5 >>> 0 < $7 >>> 0) {
      continue
     }
     break;
    };
    HEAP8[19836] = $2;
   }
  }
  _mix_pool_bytes($0, $1);
  _mix_pool_bytes($4 + 12 | 0, 4);
  global$0 = $4 + 16 | 0;
 }
 
 function _mix_pool_bytes($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $3 = HEAPU16[9906];
  $5 = HEAPU16[9907];
  if ($1) {
   $2 = HEAP32[4944];
   $4 = HEAP32[$2 + 4 >> 2] + -1 | 0;
   $8 = HEAP32[$2 + 36 >> 2];
   $9 = HEAP32[$2 + 32 >> 2];
   $10 = HEAP32[$2 + 28 >> 2];
   $11 = HEAP32[$2 + 24 >> 2];
   $12 = HEAP32[$2 + 20 >> 2];
   $2 = HEAP32[4945];
   while (1) {
    $3 = $3 + -1 & $4;
    $6 = $2 + ($3 << 2) | 0;
    $7 = __wasm_rotl_i32(HEAP8[$0 | 0], $5) ^ HEAP32[$6 >> 2] ^ HEAP32[$2 + (($3 + $12 & $4) << 2) >> 2] ^ HEAP32[$2 + (($3 + $11 & $4) << 2) >> 2] ^ HEAP32[$2 + (($3 + $10 & $4) << 2) >> 2] ^ HEAP32[$2 + (($3 + $9 & $4) << 2) >> 2] ^ HEAP32[$2 + (($3 + $8 & $4) << 2) >> 2];
    HEAP32[$6 >> 2] = HEAP32[(($7 & 7) << 2) + 31920 >> 2] ^ $7 >>> 3;
    $0 = $0 + 1 | 0;
    $5 = ($3 ? 7 : 14) + $5 & 31;
    $1 = $1 + -1 | 0;
    if ($1) {
     continue
    }
    break;
   };
  }
  HEAP16[9906] = $3;
  HEAP16[9907] = $5;
 }
 
 function strcpy($0, $1) {
  var $2 = 0, $3 = 0;
  while (1) {
   $3 = HEAPU8[$1 + $2 | 0];
   HEAP8[$0 + $2 | 0] = $3;
   $2 = $2 + 1 | 0;
   if ($3) {
    continue
   }
   break;
  };
 }
 
 function strlcpy() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0;
  $2 = 140560;
  $0 = 140559;
  while (1) {
   $0 = $0 + 1 | 0;
   if (HEAPU8[$0 | 0]) {
    continue
   }
   break;
  };
  $1 = $0 - 140560 | 0;
  $1 = $1 >>> 0 < 512 ? $1 : 511;
  if ($1) {
   $0 = 140048;
   $3 = $1;
   while (1) {
    HEAP8[$0 | 0] = HEAPU8[$2 | 0];
    $0 = $0 + 1 | 0;
    $2 = $2 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  HEAP8[$1 + 140048 | 0] = 0;
 }
 
 function strlen($0) {
  var $1 = 0;
  $1 = $0 + -1 | 0;
  while (1) {
   $1 = $1 + 1 | 0;
   if (HEAPU8[$1 | 0]) {
    continue
   }
   break;
  };
  return $1 - $0 | 0;
 }
 
 function memcpy($0, $1, $2) {
  if ($2) {
   while (1) {
    HEAP8[$0 | 0] = HEAPU8[$1 | 0];
    $0 = $0 + 1 | 0;
    $1 = $1 + 1 | 0;
    $2 = $2 + -1 | 0;
    if ($2) {
     continue
    }
    break;
   }
  }
 }
 
 function strcmp($0, $1) {
  var $2 = 0, $3 = 0;
  label$1 : {
   while (1) {
    $2 = HEAPU8[$0 | 0];
    $3 = HEAPU8[$1 | 0];
    if (($2 | 0) != ($3 | 0)) {
     break label$1
    }
    $0 = $0 + 1 | 0;
    $1 = $1 + 1 | 0;
    if ($2) {
     continue
    }
    break;
   };
   return 0;
  }
  return $2 >>> 0 < $3 >>> 0 ? -1 : 1;
 }
 
 function strncmp($0, $1, $2) {
  var $3 = 0, $4 = 0;
  label$1 : {
   label$2 : {
    if (!$2) {
     break label$2
    }
    while (1) {
     $3 = HEAPU8[$0 | 0];
     $4 = HEAPU8[$1 | 0];
     if (($3 | 0) != ($4 | 0)) {
      break label$1
     }
     if (!$3) {
      break label$2
     }
     $1 = $1 + 1 | 0;
     $0 = $0 + 1 | 0;
     $2 = $2 + -1 | 0;
     if ($2) {
      continue
     }
     break;
    };
   }
   return 0;
  }
  return $3 >>> 0 < $4 >>> 0 ? -1 : 1;
 }
 
 function strchr($0) {
  var $1 = 0, $2 = 0;
  $1 = HEAP8[$0 | 0];
  if (($1 | 0) == 46) {
   return $0
  }
  label$2 : {
   while (1) {
    if (!($1 & 255)) {
     break label$2
    }
    $1 = HEAP8[$0 + 1 | 0];
    $2 = $0 + 1 | 0;
    $0 = $2;
    if (($1 | 0) != 46) {
     continue
    }
    break;
   };
   return $2;
  }
  return 0;
 }
 
 function skip_spaces($0) {
  $0 = $0 + -1 | 0;
  while (1) {
   $0 = $0 + 1 | 0;
   if (HEAPU8[HEAPU8[$0 | 0] + 31952 | 0] & 32) {
    continue
   }
   break;
  };
  return $0;
 }
 
 function strcspn() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $1 = 3291;
  $0 = HEAPU8[3291];
  if ($0) {
   $4 = HEAPU8[32754];
   $6 = $4;
   while (1) {
    label$2 : {
     $2 = 32755;
     $3 = $4;
     if ($6) {
      while (1) {
       if (($3 & 255) == ($0 | 0)) {
        break label$2
       }
       $3 = HEAPU8[$2 | 0];
       $2 = $2 + 1 | 0;
       if ($3) {
        continue
       }
       break;
      }
     }
     $5 = $5 + 1 | 0;
     $0 = HEAPU8[$1 + 1 | 0];
     $1 = $1 + 1 | 0;
     if ($0) {
      continue
     }
    }
    break;
   };
   return $5;
  }
  return 0;
 }
 
 function memset($0, $1, $2) {
  if ($2) {
   while (1) {
    HEAP8[$0 | 0] = $1;
    $0 = $0 + 1 | 0;
    $2 = $2 + -1 | 0;
    if ($2) {
     continue
    }
    break;
   }
  }
 }
 
 function memchr($0, $1) {
  $0 = $0 + -1 | 0;
  label$1 : {
   while (1) {
    if (!$1) {
     break label$1
    }
    $1 = $1 + -1 | 0;
    $0 = $0 + 1 | 0;
    if (HEAPU8[$0 | 0] != 10) {
     continue
    }
    break;
   };
   return $0;
  }
  return 0;
 }
 
 function put_dec($0, $1, $2) {
  var $3 = 0, $4 = 0;
  if (!(!$2 & $1 >>> 0 > 99999999 | $2 >>> 0 > 0)) {
   return put_dec_trunc8($0, $1)
  }
  $3 = $2 & 65535;
  $2 = $2 >>> 16 | 0;
  $4 = $1 >>> 16 | 0;
  $1 = put_dec_helper4($0 + 8 | 0, (Math_imul($3, 42) + Math_imul($2, 4749) | 0) + put_dec_helper4($0 + 4 | 0, ((Math_imul($4, 6) + Math_imul($2, 7671) | 0) + Math_imul($3, 9496) | 0) + put_dec_helper4($0, ((Math_imul($2, 656) + ($1 & 65535) | 0) + Math_imul($4, 5536) | 0) + Math_imul($3, 7296) | 0) | 0) | 0) + Math_imul($2, 281) | 0;
  if ($1) {
   return put_dec_trunc8($0 + 12 | 0, $1)
  }
  $0 = $0 + 13 | 0;
  while (1) {
   $1 = $0 + -2 | 0;
   $2 = $0 + -1 | 0;
   $0 = $2;
   if (HEAPU8[$1 | 0] == 48) {
    continue
   }
   break;
  };
  return $2;
 }
 
 function put_dec_trunc8($0, $1) {
  var $2 = 0;
  label$1 : {
   if ($1 >>> 0 < 100) {
    break label$1
   }
   $2 = $1;
   __wasm_i64_mul($1, 0, 42949673, 0);
   $1 = i64toi32_i32$HIGH_BITS;
   HEAP16[$0 >> 1] = HEAPU16[($2 + Math_imul($1, -100) << 1) + 32224 >> 1];
   if ($1 >>> 0 < 100) {
    $0 = $0 + 2 | 0;
    break label$1;
   }
   __wasm_i64_mul($1, 0, 42949673, 0);
   $2 = i64toi32_i32$HIGH_BITS;
   HEAP16[$0 + 2 >> 1] = HEAPU16[(Math_imul($2, -100) + $1 << 1) + 32224 >> 1];
   if ($2 >>> 0 < 100) {
    $0 = $0 + 4 | 0;
    $1 = $2;
    break label$1;
   }
   $1 = Math_imul($2, 5243) >>> 19 | 0;
   HEAP16[$0 + 4 >> 1] = HEAPU16[($2 + Math_imul($1, -100) << 1) + 32224 >> 1];
   $0 = $0 + 6 | 0;
  }
  HEAP16[$0 >> 1] = HEAPU16[($1 << 1) + 32224 >> 1];
  return ($1 >>> 0 < 10 ? 1 : 2) + $0 | 0;
 }
 
 function put_dec_helper4($0, $1) {
  var $2 = 0, $3 = 0;
  $2 = $1;
  __wasm_i64_mul($1, 0, 879609303, 0);
  $1 = i64toi32_i32$HIGH_BITS >>> 11 | 0;
  $2 = $2 + Math_imul($1, -1e4) | 0;
  $3 = Math_imul($2, 5243) >>> 19 | 0;
  HEAP16[$0 + 2 >> 1] = HEAPU16[($3 << 1) + 32224 >> 1];
  HEAP16[$0 >> 1] = HEAPU16[($2 + Math_imul($3, -100) << 1) + 32224 >> 1];
  return $1;
 }
 
 function vsnprintf($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  HEAP32[$5 + 24 >> 2] = 0;
  HEAP32[$5 + 28 >> 2] = 0;
  if (($1 | 0) >= 0) {
   $11 = $0 + $1 | 0;
   $7 = $0;
   label$2 : {
    if (!HEAPU8[$2 | 0]) {
     break label$2
    }
    while (1) {
     $8 = $2;
     $9 = format_decode($8, $5 + 24 | 0);
     $2 = $9 + $8 | 0;
     label$4 : {
      label$5 : {
       label$6 : {
        label$7 : {
         $4 = HEAP32[$5 + 28 >> 2];
         label$8 : {
          label$9 : {
           label$10 : {
            label$11 : {
             label$12 : {
              label$13 : {
               label$14 : {
                $6 = HEAP32[$5 + 24 >> 2];
                $10 = $6 & 255;
                if ($10 >>> 0 > 18) {
                 break label$14
                }
                label$15 : {
                 label$16 : {
                  label$17 : {
                   label$18 : {
                    label$19 : {
                     label$20 : {
                      label$21 : {
                       label$22 : {
                        label$23 : {
                         label$24 : {
                          switch ($10 - 1 | 0) {
                          default:
                           if ($7 >>> 0 < $11 >>> 0) {
                            $4 = $11 - $7 | 0;
                            memcpy($7, $8, ($9 | 0) > ($4 | 0) ? $4 : $9);
                           }
                           $7 = $7 + $9 | 0;
                           if (HEAPU8[$2 | 0]) {
                            continue
                           }
                           break label$2;
                          case 0:
                           $9 = $6 & 255;
                           $8 = $4;
                           $4 = HEAP32[$3 >> 2];
                           $6 = $4;
                           if (($4 | 0) != $4 << 8 >> 8) {
                            break label$7
                           }
                           $4 = $6 << 8;
                           break label$4;
                          case 1:
                           $9 = $6;
                           $10 = $4 & 65535;
                           $4 = HEAP32[$3 >> 2];
                           $8 = $4;
                           $4 = $4 << 16;
                           if (($8 | 0) != $4 >> 16) {
                            break label$6
                           }
                           break label$5;
                          case 2:
                           label$31 : {
                            if ($4 & 2) {
                             break label$31
                            }
                            $10 = $4;
                            $4 = $6;
                            $8 = $4 >> 31;
                            $9 = ($4 >> 8) + -1 | 0;
                            $4 = $8;
                            $6 = $9 << 8 & -256 | $6 & 255;
                            HEAP32[$5 + 24 >> 2] = $6;
                            $4 = $10;
                            HEAP32[$5 + 28 >> 2] = $4;
                            if ($9 << 8 >> 8 < 1) {
                             break label$31
                            }
                            while (1) {
                             if ($7 >>> 0 < $11 >>> 0) {
                              HEAP8[$7 | 0] = 32;
                              $6 = HEAP32[$5 + 24 >> 2];
                              $4 = HEAP32[$5 + 28 >> 2];
                             }
                             $10 = $4;
                             $4 = $6;
                             $8 = $4 >> 31;
                             $9 = ($4 >> 8) + -1 | 0;
                             $4 = $8;
                             $6 = $9 << 8 & -256 | $6 & 255;
                             HEAP32[$5 + 24 >> 2] = $6;
                             $4 = $10;
                             HEAP32[$5 + 28 >> 2] = $4;
                             $7 = $7 + 1 | 0;
                             if ($9 << 8 >> 8 > 0) {
                              continue
                             }
                             break;
                            };
                           }
                           if ($7 >>> 0 < $11 >>> 0) {
                            HEAP8[$7 | 0] = HEAP32[$3 >> 2];
                            $6 = HEAP32[$5 + 24 >> 2];
                            $4 = HEAP32[$5 + 28 >> 2];
                           }
                           $3 = $3 + 4 | 0;
                           $10 = $4;
                           $4 = $6;
                           $8 = $4 >> 31;
                           $9 = ($4 >> 8) + -1 | 0;
                           $4 = $8;
                           $6 = $9 << 8 & -256 | $6 & 255;
                           HEAP32[$5 + 24 >> 2] = $6;
                           $4 = $10;
                           HEAP32[$5 + 28 >> 2] = $4;
                           $7 = $7 + 1 | 0;
                           if ($9 << 8 >> 8 >= 1) {
                            while (1) {
                             if ($7 >>> 0 < $11 >>> 0) {
                              HEAP8[$7 | 0] = 32;
                              $6 = HEAP32[$5 + 24 >> 2];
                              $4 = HEAP32[$5 + 28 >> 2];
                             }
                             $10 = $4;
                             $4 = $6;
                             $8 = $4 >> 31;
                             $9 = ($4 >> 8) + -1 | 0;
                             $4 = $8;
                             $6 = $9 << 8 & -256 | $6 & 255;
                             HEAP32[$5 + 24 >> 2] = $6;
                             $4 = $10;
                             HEAP32[$5 + 28 >> 2] = $4;
                             $7 = $7 + 1 | 0;
                             if ($9 << 8 >> 8 > 0) {
                              continue
                             }
                             break;
                            }
                           }
                           if (HEAPU8[$2 | 0]) {
                            continue
                           }
                           break label$2;
                          case 3:
                           $4 = HEAP32[$3 >> 2];
                           $8 = HEAP32[$5 + 28 >> 2];
                           HEAP32[$5 + 8 >> 2] = HEAP32[$5 + 24 >> 2];
                           HEAP32[$5 + 12 >> 2] = $8;
                           $7 = string($7, $11, $4, $5 + 8 | 0);
                           break label$23;
                          case 17:
                           break label$11;
                          case 16:
                           break label$12;
                          case 15:
                           break label$13;
                          case 14:
                           break label$14;
                          case 13:
                           break label$15;
                          case 12:
                           break label$16;
                          case 11:
                           break label$17;
                          case 10:
                           break label$18;
                          case 9:
                           break label$19;
                          case 6:
                           break label$2;
                          case 8:
                           break label$20;
                          case 7:
                           break label$21;
                          case 5:
                           break label$22;
                          case 4:
                           break label$24;
                          };
                         }
                         $4 = HEAP32[$3 >> 2];
                         $8 = HEAP32[$5 + 28 >> 2];
                         HEAP32[$5 + 16 >> 2] = HEAP32[$5 + 24 >> 2];
                         HEAP32[$5 + 20 >> 2] = $8;
                         $7 = pointer($2, $7, $11, $4, $5 + 16 | 0);
                         $2 = $2 + -1 | 0;
                         while (1) {
                          $2 = $2 + 1 | 0;
                          if (HEAPU8[HEAPU8[$2 | 0] + 31952 | 0] & 7) {
                           continue
                          }
                          break;
                         };
                        }
                        $3 = $3 + 4 | 0;
                        if (HEAPU8[$2 | 0]) {
                         continue
                        }
                        break label$2;
                       }
                       if ($7 >>> 0 < $11 >>> 0) {
                        HEAP8[$7 | 0] = 37
                       }
                       $7 = $7 + 1 | 0;
                       if (HEAPU8[$2 | 0]) {
                        continue
                       }
                       break label$2;
                      }
                      $4 = $3 + 7 & -8;
                      $3 = $4 + 8 | 0;
                      $6 = HEAP32[$4 >> 2];
                      $4 = HEAP32[$4 + 4 >> 2];
                      break label$8;
                     }
                     $6 = HEAP32[$3 >> 2];
                     $4 = 0;
                     $3 = $3 + 4 | 0;
                     break label$8;
                    }
                    $4 = HEAP32[$3 >> 2];
                    $6 = $4;
                    $4 = $4 >> 31;
                    $3 = $3 + 4 | 0;
                    break label$8;
                   }
                   $6 = HEAPU8[$3 | 0];
                   $4 = 0;
                   $3 = $3 + 4 | 0;
                   break label$8;
                  }
                  $4 = HEAP8[$3 | 0];
                  $6 = $4;
                  $4 = $4 >> 31;
                  $3 = $3 + 4 | 0;
                  break label$8;
                 }
                 $6 = HEAPU16[$3 >> 1];
                 $4 = 0;
                 $3 = $3 + 4 | 0;
                 break label$8;
                }
                $4 = HEAP16[$3 >> 1];
                $6 = $4;
                $4 = $4 >> 31;
                $3 = $3 + 4 | 0;
                break label$8;
               }
               $6 = HEAP32[$3 >> 2];
               $4 = 0;
               $3 = $3 + 4 | 0;
               break label$8;
              }
              $4 = HEAP32[$3 >> 2];
              $6 = $4;
              $4 = $4 >> 31;
              $3 = $3 + 4 | 0;
              break label$8;
             }
             $8 = $3 + 4 | 0;
             $6 = HEAP32[$3 >> 2];
             if ($4 & 1) {
              break label$10
             }
             $4 = 0;
             break label$9;
            }
            $4 = HEAP32[$3 >> 2];
            $6 = $4;
            $4 = $4 >> 31;
            $3 = $3 + 4 | 0;
            break label$8;
           }
           $4 = $6 >> 31;
          }
          $3 = $8;
         }
         $8 = HEAP32[$5 + 28 >> 2];
         HEAP32[$5 >> 2] = HEAP32[$5 + 24 >> 2];
         HEAP32[$5 + 4 >> 2] = $8;
         $7 = number($7, $11, $6, $4, $5);
         if (HEAPU8[$2 | 0]) {
          continue
         }
         break label$2;
        }
        $4 = ($6 | 0) > -8388607 ? $6 : -8388607;
        $4 = (($4 | 0) < 8388607 ? $4 : 8388607) << 8;
        break label$4;
       }
       $4 = ($8 | 0) > 0 ? $8 : 0;
       $4 = (($4 | 0) < 32767 ? $4 : 32767) << 16;
      }
      $3 = $3 + 4 | 0;
      HEAP32[$5 + 24 >> 2] = $9;
      HEAP32[$5 + 28 >> 2] = $4 | $10;
      if (HEAPU8[$2 | 0]) {
       continue
      }
      break label$2;
     }
     $3 = $3 + 4 | 0;
     HEAP32[$5 + 24 >> 2] = $4 | $9;
     HEAP32[$5 + 28 >> 2] = $8;
     if (HEAPU8[$2 | 0]) {
      continue
     }
     break;
    };
   }
   if ($1) {
    HEAP8[($7 >>> 0 < $11 >>> 0 ? $7 : $11 + -1 | 0) | 0] = 0
   }
   $7 = $7 - $0 | 0;
  }
  global$0 = $5 + 32 | 0;
  return $7;
 }
 
 function format_decode($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $8 = global$0 - 16 | 0;
  global$0 = $8;
  HEAP32[$8 + 12 >> 2] = $0;
  $11 = $8;
  $2 = HEAPU8[$1 + 4 | 0] | HEAPU8[$1 + 5 | 0] << 8 | (HEAPU8[$1 + 6 | 0] << 16 | HEAPU8[$1 + 7 | 0] << 24);
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        $3 = HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8 | (HEAPU8[$1 + 2 | 0] << 16 | HEAPU8[$1 + 3 | 0] << 24);
        $4 = $3 & 255;
        if (($4 | 0) != 2) {
         if (($4 | 0) != 1) {
          break label$6
         }
         $6 = $3 >> 8;
         if (($6 | 0) <= -1) {
          $5 = $2 & -3 | 2;
          $2 = $5;
          $5 = $3 & 255;
          $3 = $6;
          $3 = $5 | 0 - ($3 << 8) & -256;
          HEAP8[$1 | 0] = $3;
          HEAP8[$1 + 1 | 0] = $3 >>> 8;
          HEAP8[$1 + 2 | 0] = $3 >>> 16;
          HEAP8[$1 + 3 | 0] = $3 >>> 24;
          HEAP8[$1 + 4 | 0] = $2;
          HEAP8[$1 + 5 | 0] = $2 >>> 8;
          HEAP8[$1 + 6 | 0] = $2 >>> 16;
          HEAP8[$1 + 7 | 0] = $2 >>> 24;
         }
         $4 = $2;
         $7 = $3 & -256;
         $2 = $7;
         HEAP8[$1 | 0] = $2;
         HEAP8[$1 + 1 | 0] = $2 >>> 8;
         HEAP8[$1 + 2 | 0] = $2 >>> 16;
         HEAP8[$1 + 3 | 0] = $2 >>> 24;
         HEAP8[$1 + 4 | 0] = $4;
         HEAP8[$1 + 5 | 0] = $4 >>> 8;
         HEAP8[$1 + 6 | 0] = $4 >>> 16;
         HEAP8[$1 + 7 | 0] = $4 >>> 24;
         $6 = $0;
         break label$5;
        }
        if ($2 >> 16 <= -1) {
         $4 = $2 & 65535;
         $2 = $4;
         HEAP8[$1 | 0] = $3;
         HEAP8[$1 + 1 | 0] = $3 >>> 8;
         HEAP8[$1 + 2 | 0] = $3 >>> 16;
         HEAP8[$1 + 3 | 0] = $3 >>> 24;
         HEAP8[$1 + 4 | 0] = $2;
         HEAP8[$1 + 5 | 0] = $2 >>> 8;
         HEAP8[$1 + 6 | 0] = $2 >>> 16;
         HEAP8[$1 + 7 | 0] = $2 >>> 24;
        }
        $4 = $2;
        $7 = $3 & -256;
        $2 = $7;
        HEAP8[$1 | 0] = $2;
        HEAP8[$1 + 1 | 0] = $2 >>> 8;
        HEAP8[$1 + 2 | 0] = $2 >>> 16;
        HEAP8[$1 + 3 | 0] = $2 >>> 24;
        HEAP8[$1 + 4 | 0] = $4;
        HEAP8[$1 + 5 | 0] = $4 >>> 8;
        HEAP8[$1 + 6 | 0] = $4 >>> 16;
        HEAP8[$1 + 7 | 0] = $4 >>> 24;
        $5 = $0;
        $6 = $5;
        break label$4;
       }
       $4 = $3 & -256;
       HEAP8[$1 | 0] = $4;
       HEAP8[$1 + 1 | 0] = $4 >>> 8;
       HEAP8[$1 + 2 | 0] = $4 >>> 16;
       HEAP8[$1 + 3 | 0] = $4 >>> 24;
       HEAP8[$1 + 4 | 0] = $2;
       HEAP8[$1 + 5 | 0] = $2 >>> 8;
       HEAP8[$1 + 6 | 0] = $2 >>> 16;
       HEAP8[$1 + 7 | 0] = $2 >>> 24;
       $5 = $0;
       while (1) {
        label$10 : {
         $7 = $0 + $6 | 0;
         $4 = HEAPU8[$7 | 0];
         if (!$4) {
          break label$10
         }
         if (($4 | 0) == 37) {
          break label$10
         }
         $5 = $7 + 1 | 0;
         HEAP32[$8 + 12 >> 2] = $5;
         $6 = $6 + 1 | 0;
         continue;
        }
        break;
       };
       if ($6) {
        break label$1
       }
       if (!$4) {
        break label$1
       }
       $6 = $0 + 1 | 0;
       $3 = $3 & -256;
       $2 = $2 & -256;
       while (1) {
        label$12 : {
         HEAP8[$1 | 0] = $3;
         HEAP8[$1 + 1 | 0] = $3 >>> 8;
         HEAP8[$1 + 2 | 0] = $3 >>> 16;
         HEAP8[$1 + 3 | 0] = $3 >>> 24;
         HEAP8[$1 + 4 | 0] = $2;
         HEAP8[$1 + 5 | 0] = $2 >>> 8;
         HEAP8[$1 + 6 | 0] = $2 >>> 16;
         HEAP8[$1 + 7 | 0] = $2 >>> 24;
         HEAP32[$8 + 12 >> 2] = $6;
         $5 = HEAP8[$6 | 0] + -32 | 0;
         if ($5 >>> 0 > 16) {
          break label$12
         }
         $4 = 2;
         label$14 : {
          label$15 : {
           switch ($5 - 1 | 0) {
           default:
            $4 = 8;
            break label$14;
           case 2:
            $4 = 64;
            break label$14;
           case 10:
            $4 = 4;
            break label$14;
           case 0:
           case 1:
           case 3:
           case 4:
           case 5:
           case 6:
           case 7:
           case 8:
           case 9:
           case 11:
           case 13:
           case 14:
            break label$12;
           case 12:
            break label$14;
           case 15:
            break label$15;
           };
          }
          $4 = 16;
         }
         $6 = $6 + 1 | 0;
         $2 = $2 | $4;
         continue;
        }
        break;
       };
       $4 = $2;
       $7 = $3 | -256;
       $2 = $7;
       HEAP8[$1 | 0] = $2;
       HEAP8[$1 + 1 | 0] = $2 >>> 8;
       HEAP8[$1 + 2 | 0] = $2 >>> 16;
       HEAP8[$1 + 3 | 0] = $2 >>> 24;
       HEAP8[$1 + 4 | 0] = $4;
       HEAP8[$1 + 5 | 0] = $4 >>> 8;
       HEAP8[$1 + 6 | 0] = $4 >>> 16;
       HEAP8[$1 + 7 | 0] = $4 >>> 24;
       $2 = HEAP8[$6 | 0];
       if ($2 + -48 >>> 0 > 9) {
        if (($2 | 0) != 42) {
         break label$5
        }
        $2 = $3 | -255;
        HEAP8[$1 | 0] = $2;
        HEAP8[$1 + 1 | 0] = $2 >>> 8;
        HEAP8[$1 + 2 | 0] = $2 >>> 16;
        HEAP8[$1 + 3 | 0] = $2 >>> 24;
        HEAP8[$1 + 4 | 0] = $4;
        HEAP8[$1 + 5 | 0] = $4 >>> 8;
        HEAP8[$1 + 6 | 0] = $4 >>> 16;
        HEAP8[$1 + 7 | 0] = $4 >>> 24;
        $5 = $6 + 1 | 0;
        break label$2;
       }
       $3 = skip_atoi($8 + 12 | 0);
       $2 = HEAPU8[$1 + 4 | 0] | HEAPU8[$1 + 5 | 0] << 8 | (HEAPU8[$1 + 6 | 0] << 16 | HEAPU8[$1 + 7 | 0] << 24);
       $4 = $2;
       $7 = (HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8 | (HEAPU8[$1 + 2 | 0] << 16 | HEAPU8[$1 + 3 | 0] << 24)) & 255 | $3 << 8;
       $3 = $7;
       HEAP8[$1 | 0] = $3;
       HEAP8[$1 + 1 | 0] = $3 >>> 8;
       HEAP8[$1 + 2 | 0] = $3 >>> 16;
       HEAP8[$1 + 3 | 0] = $3 >>> 24;
       HEAP8[$1 + 4 | 0] = $2;
       HEAP8[$1 + 5 | 0] = $2 >>> 8;
       HEAP8[$1 + 6 | 0] = $2 >>> 16;
       HEAP8[$1 + 7 | 0] = $2 >>> 24;
       $6 = HEAP32[$8 + 12 >> 2];
      }
      $2 = $4 | -65536;
      $4 = $2;
      $3 = $7;
      HEAP8[$1 | 0] = $3;
      HEAP8[$1 + 1 | 0] = $3 >>> 8;
      HEAP8[$1 + 2 | 0] = $3 >>> 16;
      HEAP8[$1 + 3 | 0] = $3 >>> 24;
      HEAP8[$1 + 4 | 0] = $2;
      HEAP8[$1 + 5 | 0] = $2 >>> 8;
      HEAP8[$1 + 6 | 0] = $2 >>> 16;
      HEAP8[$1 + 7 | 0] = $2 >>> 24;
      label$20 : {
       label$21 : {
        $9 = HEAPU8[$6 | 0];
        if (($9 | 0) == 46) {
         $5 = $6 + 1 | 0;
         HEAP32[$8 + 12 >> 2] = $5;
         $2 = HEAP8[$6 + 1 | 0];
         if ($2 + -48 >>> 0 <= 9) {
          break label$21
         }
         if (($2 | 0) != 42) {
          break label$20
         }
         $2 = $7 & -256 | 2;
         HEAP8[$1 | 0] = $2;
         HEAP8[$1 + 1 | 0] = $2 >>> 8;
         HEAP8[$1 + 2 | 0] = $2 >>> 16;
         HEAP8[$1 + 3 | 0] = $2 >>> 24;
         HEAP8[$1 + 4 | 0] = $4;
         HEAP8[$1 + 5 | 0] = $4 >>> 8;
         HEAP8[$1 + 6 | 0] = $4 >>> 16;
         HEAP8[$1 + 7 | 0] = $4 >>> 24;
         $5 = $6 + 2 | 0;
         break label$2;
        }
        $5 = $6;
        break label$3;
       }
       $2 = skip_atoi($8 + 12 | 0) << 16;
       $3 = $2 >> 16 < 0;
       $7 = 0 | (HEAPU8[$1 | 0] | HEAPU8[$1 + 1 | 0] << 8 | (HEAPU8[$1 + 2 | 0] << 16 | HEAPU8[$1 + 3 | 0] << 24));
       $3 = (HEAPU8[$1 + 4 | 0] | HEAPU8[$1 + 5 | 0] << 8 | (HEAPU8[$1 + 6 | 0] << 16 | HEAPU8[$1 + 7 | 0] << 24)) & 65535 | ($3 ? 0 : $2);
       $4 = $3;
       HEAP8[$1 | 0] = $7;
       HEAP8[$1 + 1 | 0] = $7 >>> 8;
       HEAP8[$1 + 2 | 0] = $7 >>> 16;
       HEAP8[$1 + 3 | 0] = $7 >>> 24;
       HEAP8[$1 + 4 | 0] = $3;
       HEAP8[$1 + 5 | 0] = $3 >>> 8;
       HEAP8[$1 + 6 | 0] = $3 >>> 16;
       HEAP8[$1 + 7 | 0] = $3 >>> 24;
       $5 = HEAP32[$8 + 12 >> 2];
      }
      $6 = $5;
     }
     $9 = HEAPU8[$6 | 0];
    }
    label$23 : {
     label$24 : {
      label$25 : {
       $2 = ($9 & 255) + -76 | 0;
       if ($2 >>> 0 <= 46) {
        $9 = $6;
        label$27 : {
         switch ($2 - 1 | 0) {
         case 0:
         case 1:
         case 2:
         case 3:
         case 4:
         case 5:
         case 6:
         case 7:
         case 8:
         case 9:
         case 10:
         case 11:
         case 12:
         case 13:
         case 14:
         case 15:
         case 16:
         case 17:
         case 18:
         case 19:
         case 20:
         case 21:
         case 22:
         case 23:
         case 24:
         case 25:
         case 26:
         case 28:
         case 29:
         case 30:
         case 32:
         case 33:
         case 34:
         case 35:
         case 36:
         case 37:
         case 38:
         case 40:
         case 41:
         case 42:
         case 43:
         case 44:
          break label$23;
         default:
          break label$27;
         };
        }
        $5 = $6 + 1 | 0;
        HEAP32[$8 + 12 >> 2] = $5;
        $10 = HEAPU8[$6 | 0];
        if (($10 | 0) == HEAPU8[$6 + 1 | 0]) {
         break label$25
        }
        break label$24;
       }
       $9 = $6;
       break label$23;
      }
      if (($10 | 0) != 104) {
       $9 = $5;
       if (($10 | 0) != 108) {
        break label$23
       }
       $5 = $6 + 2 | 0;
       HEAP32[$8 + 12 >> 2] = $5;
       $10 = 76;
       break label$24;
      }
      $5 = $6 + 2 | 0;
      HEAP32[$8 + 12 >> 2] = $5;
      $10 = 72;
     }
     $9 = $5;
    }
    $2 = $4 & -65281;
    $12 = $2;
    $6 = $2 | 2560;
    $2 = $6;
    $3 = $7;
    HEAP8[$1 | 0] = $3;
    HEAP8[$1 + 1 | 0] = $3 >>> 8;
    HEAP8[$1 + 2 | 0] = $3 >>> 16;
    HEAP8[$1 + 3 | 0] = $3 >>> 24;
    HEAP8[$1 + 4 | 0] = $2;
    HEAP8[$1 + 5 | 0] = $2 >>> 8;
    HEAP8[$1 + 6 | 0] = $2 >>> 16;
    HEAP8[$1 + 7 | 0] = $2 >>> 24;
    label$29 : {
     label$30 : {
      label$31 : {
       label$32 : {
        label$33 : {
         label$34 : {
          label$35 : {
           label$36 : {
            label$37 : {
             $6 = HEAP8[$9 | 0] + -37 | 0;
             if ($6 >>> 0 > 83) {
              break label$37
             }
             label$38 : {
              switch ($6 - 1 | 0) {
              case 62:
              case 67:
               $2 = $4 & 254 | $2 & -62976 | 1;
               break label$30;
              case 79:
               break label$29;
              case 50:
               break label$31;
              case 82:
               break label$32;
              case 77:
               break label$33;
              case 74:
               break label$34;
              case 73:
               break label$35;
              case 61:
               break label$36;
              case 0:
              case 1:
              case 2:
              case 3:
              case 4:
              case 5:
              case 6:
              case 7:
              case 8:
              case 9:
              case 10:
              case 11:
              case 12:
              case 13:
              case 14:
              case 15:
              case 16:
              case 17:
              case 18:
              case 19:
              case 20:
              case 21:
              case 22:
              case 23:
              case 24:
              case 25:
              case 26:
              case 27:
              case 28:
              case 29:
              case 30:
              case 31:
              case 32:
              case 33:
              case 34:
              case 35:
              case 36:
              case 37:
              case 38:
              case 39:
              case 40:
              case 41:
              case 42:
              case 43:
              case 44:
              case 45:
              case 46:
              case 47:
              case 48:
              case 49:
              case 51:
              case 52:
              case 53:
              case 54:
              case 55:
              case 56:
              case 57:
              case 58:
              case 59:
              case 60:
              case 63:
              case 64:
              case 65:
              case 66:
              case 68:
              case 69:
              case 70:
              case 71:
              case 72:
              case 75:
              case 76:
              case 78:
              case 80:
              case 81:
               break label$37;
              default:
               break label$38;
              };
             }
             $3 = $3 & -256 | 6;
             HEAP8[$1 | 0] = $3;
             HEAP8[$1 + 1 | 0] = $3 >>> 8;
             HEAP8[$1 + 2 | 0] = $3 >>> 16;
             HEAP8[$1 + 3 | 0] = $3 >>> 24;
             $2 = $2 & -62721;
             HEAP8[$1 + 4 | 0] = $2;
             HEAP8[$1 + 5 | 0] = $2 >>> 8;
             HEAP8[$1 + 6 | 0] = $2 >>> 16;
             HEAP8[$1 + 7 | 0] = $2 >>> 24;
             $5 = $9 + 1 | 0;
             break label$2;
            }
            $3 = $3 & -256 | 7;
            HEAP8[$1 | 0] = $3;
            HEAP8[$1 + 1 | 0] = $3 >>> 8;
            HEAP8[$1 + 2 | 0] = $3 >>> 16;
            HEAP8[$1 + 3 | 0] = $3 >>> 24;
            $2 = $2 & -62721;
            HEAP8[$1 + 4 | 0] = $2;
            HEAP8[$1 + 5 | 0] = $2 >>> 8;
            HEAP8[$1 + 6 | 0] = $2 >>> 16;
            HEAP8[$1 + 7 | 0] = $2 >>> 24;
            break label$1;
           }
           $3 = $3 & -256 | 3;
           HEAP8[$1 | 0] = $3;
           HEAP8[$1 + 1 | 0] = $3 >>> 8;
           HEAP8[$1 + 2 | 0] = $3 >>> 16;
           HEAP8[$1 + 3 | 0] = $3 >>> 24;
           $2 = $2 & -62721;
           HEAP8[$1 + 4 | 0] = $2;
           HEAP8[$1 + 5 | 0] = $2 >>> 8;
           HEAP8[$1 + 6 | 0] = $2 >>> 16;
           HEAP8[$1 + 7 | 0] = $2 >>> 24;
           $5 = $9 + 1 | 0;
           break label$2;
          }
          $2 = $12 | 2048;
          break label$30;
         }
         $3 = $3 & -256 | 5;
         HEAP8[$1 | 0] = $3;
         HEAP8[$1 + 1 | 0] = $3 >>> 8;
         HEAP8[$1 + 2 | 0] = $3 >>> 16;
         HEAP8[$1 + 3 | 0] = $3 >>> 24;
         $2 = $2 & -62721;
         HEAP8[$1 + 4 | 0] = $2;
         HEAP8[$1 + 5 | 0] = $2 >>> 8;
         HEAP8[$1 + 6 | 0] = $2 >>> 16;
         HEAP8[$1 + 7 | 0] = $2 >>> 24;
         $5 = $9 + 1 | 0;
         break label$2;
        }
        $3 = $3 & -256 | 4;
        HEAP8[$1 | 0] = $3;
        HEAP8[$1 + 1 | 0] = $3 >>> 8;
        HEAP8[$1 + 2 | 0] = $3 >>> 16;
        HEAP8[$1 + 3 | 0] = $3 >>> 24;
        $2 = $2 & -62721;
        HEAP8[$1 + 4 | 0] = $2;
        HEAP8[$1 + 5 | 0] = $2 >>> 8;
        HEAP8[$1 + 6 | 0] = $2 >>> 16;
        HEAP8[$1 + 7 | 0] = $2 >>> 24;
        $5 = $9 + 1 | 0;
        break label$2;
       }
       $4 = $4 & 223 | $2 & -62976 | 32;
       $2 = $4;
       HEAP8[$1 | 0] = $3;
       HEAP8[$1 + 1 | 0] = $3 >>> 8;
       HEAP8[$1 + 2 | 0] = $3 >>> 16;
       HEAP8[$1 + 3 | 0] = $3 >>> 24;
       HEAP8[$1 + 4 | 0] = $2;
       HEAP8[$1 + 5 | 0] = $2 >>> 8;
       HEAP8[$1 + 6 | 0] = $2 >>> 16;
       HEAP8[$1 + 7 | 0] = $2 >>> 24;
      }
      $2 = $2 & -65281 | 4096;
     }
     HEAP8[$1 | 0] = $3;
     HEAP8[$1 + 1 | 0] = $3 >>> 8;
     HEAP8[$1 + 2 | 0] = $3 >>> 16;
     HEAP8[$1 + 3 | 0] = $3 >>> 24;
     HEAP8[$1 + 4 | 0] = $2;
     HEAP8[$1 + 5 | 0] = $2 >>> 8;
     HEAP8[$1 + 6 | 0] = $2 >>> 16;
     HEAP8[$1 + 7 | 0] = $2 >>> 24;
    }
    label$40 : {
     label$41 : {
      label$42 : {
       label$43 : {
        label$44 : {
         if (($10 | 0) > 107) {
          if (($10 | 0) == 108) {
           break label$44
          }
          if (($10 | 0) == 116) {
           break label$43
          }
          if (($10 | 0) != 122) {
           break label$41
          }
          $3 = $3 & -256;
          $7 = 17;
          $4 = 0;
          break label$40;
         }
         if (($10 | 0) == 72) {
          break label$42
         }
         if (($10 | 0) != 76) {
          break label$41
         }
         $3 = $3 & -256;
         $7 = 8;
         $4 = 0;
         break label$40;
        }
        $7 = $3 & -256;
        $4 = $2;
        $2 = 0;
        $3 = ($4 & 1) + 9 | 0;
        if ($3 >>> 0 < 9) {
         $2 = 1
        }
        break label$40;
       }
       $3 = $3 & -256;
       $7 = 18;
       $4 = 0;
       break label$40;
      }
      $7 = $3 & -256;
      $5 = 0;
      $4 = $2;
      $2 = ($2 & 1) + 11 | 0;
      if ($2 >>> 0 < 11) {
       $5 = 1
      }
      $3 = $2;
      $2 = $5;
      break label$40;
     }
     $4 = $2 & 1;
     if (($10 | 0) == 104) {
      $3 = $3 & -256;
      $7 = $4 + 13 | 0;
      $4 = 0;
      break label$40;
     }
     $3 = $3 & -256;
     $7 = $4 + 15 | 0;
     $4 = 0;
    }
    $3 = $3 | $7;
    HEAP8[$1 | 0] = $3;
    HEAP8[$1 + 1 | 0] = $3 >>> 8;
    HEAP8[$1 + 2 | 0] = $3 >>> 16;
    HEAP8[$1 + 3 | 0] = $3 >>> 24;
    $2 = $2 | $4;
    HEAP8[$1 + 4 | 0] = $2;
    HEAP8[$1 + 5 | 0] = $2 >>> 8;
    HEAP8[$1 + 6 | 0] = $2 >>> 16;
    HEAP8[$1 + 7 | 0] = $2 >>> 24;
    $5 = $9 + 1 | 0;
   }
   HEAP32[$11 + 12 >> 2] = $5;
  }
  global$0 = $8 + 16 | 0;
  return $5 - $0 | 0;
 }
 
 function string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  label$1 : {
   label$2 : {
    $6 = (HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24)) >> 16;
    if ($6) {
     $8 = $2 >>> 0 < 65536 ? 32208 : $2;
     $2 = 0;
     while (1) {
      $5 = $0 + $2 | 0;
      $7 = HEAPU8[$2 + $8 | 0];
      if (!$7) {
       break label$2
      }
      if ($5 >>> 0 < $1 >>> 0) {
       HEAP8[$5 | 0] = $7
      }
      $2 = $2 + 1 | 0;
      if (($6 | 0) != ($2 | 0)) {
       continue
      }
      break;
     };
     $0 = $0 + $2 | 0;
     break label$1;
    }
    $2 = 0;
    break label$1;
   }
   $0 = $5;
  }
  $5 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$4 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$4 + 12 >> 2] = $5;
  $0 = widen_string($0, $2, $1, $4 + 8 | 0);
  global$0 = $4 + 16 | 0;
  return $0;
 }
 
 function pointer($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0;
  $5 = global$0 - 208 | 0;
  global$0 = $5;
  $6 = HEAP8[$0 | 0];
  label$1 : {
   label$2 : {
    if ($3) {
     break label$2
    }
    $7 = $6 & 255;
    if (($7 | 0) == 75) {
     break label$2
    }
    if (($7 | 0) == 120) {
     break label$2
    }
    $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
    $3 = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
    if ($3 >> 8 == -1) {
     $3 = $3 & 255 | 2048;
     HEAP8[$4 | 0] = $3;
     HEAP8[$4 + 1 | 0] = $3 >>> 8;
     HEAP8[$4 + 2 | 0] = $3 >>> 16;
     HEAP8[$4 + 3 | 0] = $3 >>> 24;
     HEAP8[$4 + 4 | 0] = $0;
     HEAP8[$4 + 5 | 0] = $0 >>> 8;
     HEAP8[$4 + 6 | 0] = $0 >>> 16;
     HEAP8[$4 + 7 | 0] = $0 >>> 24;
    }
    $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
    HEAP32[$5 + 8 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
    HEAP32[$5 + 12 >> 2] = $0;
    $0 = string($1, $2, 32208, $5 + 8 | 0);
    break label$1;
   }
   label$4 : {
    label$5 : {
     label$6 : {
      label$7 : {
       label$8 : {
        label$9 : {
         label$10 : {
          label$11 : {
           label$12 : {
            label$13 : {
             label$14 : {
              label$15 : {
               label$16 : {
                label$17 : {
                 label$18 : {
                  $6 = $6 + -66 | 0;
                  if ($6 >>> 0 > 54) {
                   break label$18
                  }
                  label$19 : {
                   switch ($6 - 1 | 0) {
                   default:
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 40 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 44 >> 2] = $6;
                    $0 = symbol_string($1, $2, $3, $5 + 40 | 0, $0);
                    break label$1;
                   case 6:
                   case 38:
                    $6 = HEAP8[$0 + 1 | 0];
                    if (($6 | 0) == 52) {
                     break label$10
                    }
                    if (($6 | 0) == 83) {
                     break label$9
                    }
                    if (($6 | 0) != 54) {
                     break label$18
                    }
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 88 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 92 >> 2] = $6;
                    $0 = ip6_addr_string($1, $2, $3, $5 + 88 | 0, $0);
                    break label$1;
                   case 10:
                   case 42:
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 80 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 84 >> 2] = $6;
                    $0 = mac_address_string($1, $2, $3, $5 + 80 | 0, $0);
                    break label$1;
                   case 15:
                   case 47:
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 48 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 52 >> 2] = $6;
                    $0 = resource_string($1, $2, $3, $5 + 48 | 0, $0);
                    break label$1;
                   case 0:
                    $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 168 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 172 >> 2] = $0;
                    $0 = clock($1, $2, $5 + 168 | 0);
                    break label$1;
                   case 1:
                    $3 = HEAP32[$3 + 12 >> 2];
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 176 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 180 >> 2] = $6;
                    $0 = dentry_name($1, $2, $3, $5 + 176 | 0, $0);
                    break label$1;
                   case 2:
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 128 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 132 >> 2] = $6;
                    $0 = escaped_string($1, $2, $3, $5 + 128 | 0, $0);
                    break label$1;
                   case 4:
                    $0 = flags_string($1, $2, $3, $0);
                    break label$1;
                   case 8:
                    if (!HEAP32[34968]) {
                     break label$18
                    }
                    $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 144 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 148 >> 2] = $0;
                    $0 = restricted_pointer($1, $2, $3, $5 + 144 | 0);
                    break label$1;
                   case 11:
                    $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                    HEAP32[$5 + 152 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                    HEAP32[$5 + 156 >> 2] = $6;
                    $0 = netdev_bits($1, $2, $3, $5 + 152 | 0, $0);
                    break label$1;
                   case 53:
                    break label$11;
                   case 37:
                    break label$12;
                   case 33:
                    break label$13;
                   case 31:
                    break label$14;
                   case 30:
                    break label$15;
                   case 19:
                    break label$16;
                   case 18:
                    break label$17;
                   case 5:
                   case 7:
                   case 9:
                   case 13:
                   case 14:
                   case 17:
                   case 20:
                   case 21:
                   case 22:
                   case 23:
                   case 24:
                   case 25:
                   case 26:
                   case 27:
                   case 28:
                   case 29:
                   case 32:
                   case 34:
                   case 36:
                   case 39:
                   case 40:
                   case 41:
                   case 43:
                   case 44:
                   case 45:
                   case 46:
                   case 49:
                   case 50:
                   case 51:
                   case 52:
                    break label$18;
                   case 12:
                    break label$19;
                   };
                  }
                  if (HEAPU8[$0 + 1 | 0] != 70) {
                   break label$18
                  }
                  $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                  HEAP32[$5 + 184 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                  HEAP32[$5 + 188 >> 2] = $0;
                  $0 = device_node_string($1, $2, $5 + 184 | 0);
                  break label$1;
                 }
                 $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                 $0 = $6;
                 $4 = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                 HEAP32[$5 + 200 >> 2] = $4;
                 HEAP32[$5 + 204 >> 2] = $0;
                 if (HEAPU8[19920]) {
                  break label$5
                 }
                 if (HEAP32[4981] >= 1) {
                  break label$4
                 }
                 $0 = siphash_1u32($3);
                 $3 = HEAP32[$5 + 204 >> 2];
                 HEAP32[$5 + 32 >> 2] = HEAP32[$5 + 200 >> 2];
                 HEAP32[$5 + 36 >> 2] = $3;
                 $0 = pointer_string($1, $2, $0, $5 + 32 | 0);
                 break label$1;
                }
                $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
                HEAP32[$5 + 136 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
                HEAP32[$5 + 140 >> 2] = $6;
                $0 = uuid_string($1, $2, $3, $5 + 136 | 0, $0);
                break label$1;
               }
               HEAP32[$5 + 200 >> 2] = HEAP32[HEAP32[$3 + 4 >> 2] >> 2];
               $0 = vsnprintf($1, $2 >>> 0 > $1 >>> 0 ? $2 - $1 | 0 : 0, HEAP32[$3 >> 2], HEAP32[$5 + 200 >> 2]) + $1 | 0;
               break label$1;
              }
              $0 = special_hex_number($1, $2, HEAP32[$3 >> 2], 0, 4);
              break label$1;
             }
             if (HEAPU8[$0 + 1 | 0] != 108) {
              break label$8
             }
             $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
             HEAP32[$5 + 64 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
             HEAP32[$5 + 68 >> 2] = $0;
             $0 = bitmap_list_string($1, $2, $3, $5 - -64 | 0);
             break label$1;
            }
            $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
            HEAP32[$5 + 160 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
            HEAP32[$5 + 164 >> 2] = $6;
            $0 = dentry_name($1, $2, $3, $5 + 160 | 0, $0);
            break label$1;
           }
           $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
           HEAP32[$5 + 56 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
           HEAP32[$5 + 60 >> 2] = $6;
           $0 = hex_string($1, $2, $3, $5 + 56 | 0, $0);
           break label$1;
          }
          $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
          HEAP32[$5 + 192 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
          HEAP32[$5 + 196 >> 2] = $0;
          $0 = pointer_string($1, $2, $3, $5 + 192 | 0);
          break label$1;
         }
         $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
         HEAP32[$5 + 96 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
         HEAP32[$5 + 100 >> 2] = $6;
         $0 = ip4_addr_string($1, $2, $3, $5 + 96 | 0, $0);
         break label$1;
        }
        $6 = HEAPU16[$3 >> 1];
        if (($6 | 0) == 10) {
         break label$7
        }
        if (($6 | 0) != 2) {
         break label$6
        }
        $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
        HEAP32[$5 + 112 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
        HEAP32[$5 + 116 >> 2] = $6;
        $0 = ip4_addr_string_sa($1, $2, $3, $5 + 112 | 0, $0);
        break label$1;
       }
       $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
       HEAP32[$5 + 72 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
       HEAP32[$5 + 76 >> 2] = $0;
       $0 = bitmap_string($1, $2, $3, $5 + 72 | 0);
       break label$1;
      }
      $6 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
      HEAP32[$5 + 120 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
      HEAP32[$5 + 124 >> 2] = $6;
      $0 = ip6_addr_string_sa($1, $2, $3, $5 + 120 | 0, $0);
      break label$1;
     }
     $0 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
     HEAP32[$5 + 104 >> 2] = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
     HEAP32[$5 + 108 >> 2] = $0;
     $0 = string($1, $2, 32424, $5 + 104 | 0);
     break label$1;
    }
    $0 = HEAP32[$5 + 204 >> 2];
    HEAP32[$5 + 16 >> 2] = HEAP32[$5 + 200 >> 2];
    HEAP32[$5 + 20 >> 2] = $0;
    $0 = pointer_string($1, $2, Math_imul($3, 1640531527), $5 + 16 | 0);
    break label$1;
   }
   $3 = $4 & 255 | 2048;
   HEAP32[$5 + 24 >> 2] = $3;
   HEAP32[$5 + 28 >> 2] = $0;
   HEAP32[$5 + 200 >> 2] = $3;
   HEAP32[$5 + 204 >> 2] = $0;
   $0 = string($1, $2, 32592, $5 + 24 | 0);
  }
  global$0 = $5 + 208 | 0;
  return $0;
 }
 
 function number($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0;
  $10 = global$0 - 32 | 0;
  global$0 = $10;
  $7 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
  $12 = ($7 & 65280) != 2560;
  $13 = ($7 & 64) >>> 6 | 0;
  $8 = HEAPU8[$4 | 0] | HEAPU8[$4 + 1 | 0] << 8 | (HEAPU8[$4 + 2 | 0] << 16 | HEAPU8[$4 + 3 | 0] << 24);
  $9 = $8 >> 8;
  $14 = $7;
  $11 = $7;
  $6 = $7;
  if ($6 & 2) {
   $6 = $7 & -17;
   $14 = $6;
   $5 = $8;
   HEAP8[$4 | 0] = $5;
   HEAP8[$4 + 1 | 0] = $5 >>> 8;
   HEAP8[$4 + 2 | 0] = $5 >>> 16;
   HEAP8[$4 + 3 | 0] = $5 >>> 24;
   HEAP8[$4 + 4 | 0] = $6;
   HEAP8[$4 + 5 | 0] = $6 >>> 8;
   HEAP8[$4 + 6 | 0] = $6 >>> 16;
   HEAP8[$4 + 7 | 0] = $6 >>> 24;
   $5 = 0;
  } else {
   $5 = 0
  }
  $15 = $12 & $13;
  $5 = 0;
  label$2 : {
   label$3 : {
    if (!($6 & 1)) {
     break label$3
    }
    label$4 : {
     if (($3 | 0) < -1) {
      $5 = 1
     } else {
      if (($3 | 0) <= -1) {
       $5 = $2 >>> 0 > 4294967295 ? 0 : 1
      } else {
       $5 = 0
      }
     }
     if (!$5) {
      if ($6 & 4) {
       break label$4
      }
      $9 = ($6 << 28 >> 31) + $9 | 0;
      $5 = $6 << 2 & 32;
      break label$3;
     }
     $9 = $9 + -1 | 0;
     $6 = 0 - $2 | 0;
     $12 = 45;
     $5 = 0 - ($3 + (0 < $2 >>> 0) | 0) | 0;
     break label$2;
    }
    $9 = $9 + -1 | 0;
    $5 = 43;
   }
   $12 = $5;
   $6 = $2;
   $5 = $3;
  }
  $7 = $7 >> 16;
  label$6 : {
   if (!$15) {
    break label$6
   }
   if (($14 & 65280) == 4096) {
    $9 = $9 + -2 | 0;
    break label$6;
   }
   $9 = $9 - (($2 | 0) != 0 | ($3 | 0) != 0) | 0;
  }
  $13 = $11 & 32;
  $11 = $7;
  $7 = $14 >>> 8 | 0;
  label$8 : {
   if (!(!$5 & $6 >>> 0 >= ($7 & 255) >>> 0 | $5 >>> 0 > 0)) {
    HEAP8[$10 + 8 | 0] = $13 | HEAPU8[$6 + 31776 | 0];
    $7 = 1;
    break label$8;
   }
   $8 = $7 & 255;
   if (($8 | 0) == 10) {
    $7 = put_dec($10 + 8 | 0, $6, $5) - ($10 + 8 | 0) | 0;
    break label$8;
   }
   $16 = ($8 | 0) == 16 ? 4 : 3;
   $17 = $7 + -1 & 255;
   $7 = 0;
   while (1) {
    HEAP8[($10 + 8 | 0) + $7 | 0] = $13 | HEAPU8[($6 & $17) + 31776 | 0];
    $7 = $7 + 1 | 0;
    $8 = $5;
    $18 = $6;
    $6 = $16 & 31;
    if (32 <= ($16 & 63) >>> 0) {
     $5 = 0;
     $6 = $8 >>> $6 | 0;
    } else {
     $5 = $8 >>> $6 | 0;
     $6 = ((1 << $6) - 1 & $8) << 32 - $6 | $18 >>> $6;
    }
    if ($6 | $5) {
     continue
    }
    break;
   };
  }
  $6 = ($7 | 0) > ($11 | 0) ? $7 : $11;
  $8 = $9 - $6 | 0;
  label$13 : {
   label$14 : {
    if (!($14 & 18)) {
     if (($8 | 0) < 1) {
      break label$14
     }
     $5 = $9 - $6 | 0;
     while (1) {
      if ($0 >>> 0 < $1 >>> 0) {
       HEAP8[$0 | 0] = 32
      }
      $0 = $0 + 1 | 0;
      $8 = -1;
      $5 = $5 + -1 | 0;
      if (($5 | 0) > 0) {
       continue
      }
      break;
     };
    }
    break label$13;
   }
   $8 = $8 + -1 | 0;
  }
  if ($12) {
   if ($0 >>> 0 < $1 >>> 0) {
    HEAP8[$0 | 0] = $12
   }
   $0 = $0 + 1 | 0;
  }
  label$19 : {
   if (!$15) {
    break label$19
   }
   $5 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
   label$20 : {
    if (($5 & 65280) != 4096) {
     if (!($2 | $3)) {
      break label$20
     }
    }
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = 48;
     $5 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
    }
    $0 = $0 + 1 | 0;
   }
   if (($5 & 65280) != 4096) {
    break label$19
   }
   if ($0 >>> 0 < $1 >>> 0) {
    HEAP8[$0 | 0] = $13 | 88
   }
   $0 = $0 + 1 | 0;
  }
  label$24 : {
   label$25 : {
    label$26 : {
     $2 = HEAPU8[$4 + 4 | 0] | HEAPU8[$4 + 5 | 0] << 8 | (HEAPU8[$4 + 6 | 0] << 16 | HEAPU8[$4 + 7 | 0] << 24);
     if (!($2 & 2)) {
      if (($8 | 0) < 1) {
       break label$26
      }
      $2 = $2 & 16 | 32;
      while (1) {
       if ($0 >>> 0 < $1 >>> 0) {
        HEAP8[$0 | 0] = $2
       }
       $0 = $0 + 1 | 0;
       $5 = -1;
       $8 = $8 + -1 | 0;
       if (($8 | 0) > 0) {
        continue
       }
       break;
      };
      if (($7 | 0) < ($11 | 0)) {
       break label$25
      }
      break label$24;
     }
     $5 = $8;
     if (($7 | 0) < ($11 | 0)) {
      break label$25
     }
     break label$24;
    }
    $5 = $8 + -1 | 0;
    if (($7 | 0) >= ($11 | 0)) {
     break label$24
    }
   }
   while (1) {
    $6 = $6 + -1 | 0;
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = 48
    }
    $0 = $0 + 1 | 0;
    if (($7 | 0) < ($6 | 0)) {
     continue
    }
    break;
   };
  }
  if (($7 | 0) >= 1) {
   $3 = $10 + 7 | 0;
   while (1) {
    $2 = $7 + -1 | 0;
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = HEAPU8[$3 + $7 | 0]
    }
    $0 = $0 + 1 | 0;
    $7 = $2;
    if (($7 | 0) > 0) {
     continue
    }
    break;
   };
  }
  if (($5 | 0) >= 1) {
   while (1) {
    $5 = $5 + -1 | 0;
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = 32
    }
    $0 = $0 + 1 | 0;
    if (($5 | 0) > 0) {
     continue
    }
    break;
   }
  }
  global$0 = $10 + 32 | 0;
  return $0;
 }
 
 function skip_atoi($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  $1 = HEAP32[$0 >> 2] + 1 | 0;
  while (1) {
   HEAP32[$0 >> 2] = $1;
   $2 = (HEAP8[$1 + -1 | 0] + Math_imul($2, 10) | 0) + -48 | 0;
   $3 = HEAP8[$1 | 0];
   $1 = $1 + 1 | 0;
   if ($3 + -48 >>> 0 < 10) {
    continue
   }
   break;
  };
  return $2;
 }
 
 function widen_string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0;
  $5 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  label$1 : {
   $4 = (HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) >> 8;
   if (($4 | 0) <= ($1 | 0)) {
    break label$1
   }
   $3 = $4 - $1 | 0;
   label$2 : {
    label$3 : {
     if (!($5 & 2)) {
      $4 = $0 - $1 | 0;
      if ($4 >>> 0 >= $2 >>> 0) {
       break label$2
      }
      $2 = $2 - $4 | 0;
      if ($2 >>> 0 <= $3 >>> 0) {
       break label$3
      }
      if ($1) {
       $2 = $2 - $3 | 0;
       memmove($3 + $4 | 0, $4, $2 >>> 0 < $1 >>> 0 ? $2 : $1);
      }
      memset($4, 32, $3);
      return $0 + $3 | 0;
     }
     if (!$3) {
      break label$1
     }
     $1 = $1 - $4 | 0;
     while (1) {
      if ($0 >>> 0 < $2 >>> 0) {
       HEAP8[$0 | 0] = 32
      }
      $0 = $0 + 1 | 0;
      $1 = $1 + 1 | 0;
      if ($1) {
       continue
      }
      break;
     };
     break label$1;
    }
    memset($4, 32, $2);
   }
   return $0 + $3 | 0;
  }
  return $0;
 }
 
 function symbol_string($0, $1, $2, $3, $4) {
  var $5 = 0;
  $5 = global$0 - 240 | 0;
  global$0 = $5;
  label$1 : {
   label$2 : {
    label$3 : {
     $4 = HEAPU8[$4 | 0];
     if (($4 | 0) == 115) {
      break label$3
     }
     if (($4 | 0) == 102) {
      break label$3
     }
     if (($4 | 0) != 66) {
      break label$2
     }
     __sprint_symbol($5 + 16 | 0, $2, -1, 1);
     break label$1;
    }
    __sprint_symbol($5 + 16 | 0, $2, 0, 0);
    break label$1;
   }
   __sprint_symbol($5 + 16 | 0, $2, 0, 1);
  }
  $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$5 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$5 + 12 >> 2] = $2;
  $0 = string($0, $1, $5 + 16 | 0, $5 + 8 | 0);
  global$0 = $5 + 240 | 0;
  return $0;
 }
 
 function ip6_addr_string($0, $1, $2, $3, $4) {
  var $5 = 0;
  $5 = global$0 + -64 | 0;
  global$0 = $5;
  label$1 : {
   label$2 : {
    if (HEAPU8[$4 | 0] != 73) {
     break label$2
    }
    if (HEAPU8[$4 + 2 | 0] != 99) {
     break label$2
    }
    ip6_compressed_string($5 + 16 | 0, $2);
    break label$1;
   }
   ip6_string($5 + 16 | 0, $2, $4);
  }
  $2 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  $3 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$5 + 8 >> 2] = $2;
  HEAP32[$5 + 12 >> 2] = $3;
  $0 = string($0, $1, $5 + 16 | 0, $5 + 8 | 0);
  global$0 = $5 - -64 | 0;
  return $0;
 }
 
 function mac_address_string($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $6 = global$0 - 48 | 0;
  global$0 = $6;
  $8 = 58;
  $10 = $6;
  $5 = HEAP8[$4 + 1 | 0];
  label$1 : {
   if (($5 | 0) != 82) {
    $8 = ($5 | 0) == 70 ? 45 : 58;
    $5 = $2;
    break label$1;
   }
   $9 = 1;
   $5 = $2 + 5 | 0;
  }
  $5 = HEAPU8[$5 | 0];
  HEAP8[$10 + 17 | 0] = HEAPU8[($5 & 15) + 31744 | 0];
  HEAP8[$6 + 16 | 0] = HEAPU8[($5 >>> 4 | 0) + 31744 | 0];
  $7 = HEAPU8[$4 | 0] != 77;
  label$4 : {
   if (!$7) {
    HEAP8[$6 + 18 | 0] = $8;
    $5 = $6 + 16 | 3;
    break label$4;
   }
   $5 = $6 + 16 | 2;
  }
  $10 = HEAPU8[($9 ? 4 : 1) + $2 | 0];
  HEAP8[$5 + 1 | 0] = HEAPU8[($10 & 15) + 31744 | 0];
  HEAP8[$5 | 0] = HEAPU8[($10 >>> 4 | 0) + 31744 | 0];
  label$6 : {
   if (!$7) {
    HEAP8[$5 + 2 | 0] = $8;
    $5 = $5 + 3 | 0;
    break label$6;
   }
   $5 = $5 + 2 | 0;
  }
  $7 = HEAPU8[($9 ? 3 : 2) + $2 | 0];
  HEAP8[$5 + 1 | 0] = HEAPU8[($7 & 15) + 31744 | 0];
  HEAP8[$5 | 0] = HEAPU8[($7 >>> 4 | 0) + 31744 | 0];
  label$8 : {
   if (HEAPU8[$4 | 0] == 77) {
    HEAP8[$5 + 2 | 0] = $8;
    $5 = $5 + 3 | 0;
    break label$8;
   }
   $5 = $5 + 2 | 0;
  }
  $7 = HEAPU8[($9 ? 2 : 3) + $2 | 0];
  HEAP8[$5 + 1 | 0] = HEAPU8[($7 & 15) + 31744 | 0];
  HEAP8[$5 | 0] = HEAPU8[($7 >>> 4 | 0) + 31744 | 0];
  label$10 : {
   if (HEAPU8[$4 | 0] == 77) {
    HEAP8[$5 + 2 | 0] = $8;
    $5 = $5 + 3 | 0;
    break label$10;
   }
   $5 = $5 + 2 | 0;
  }
  $7 = HEAPU8[($9 ? 1 : 4) + $2 | 0];
  HEAP8[$5 + 1 | 0] = HEAPU8[($7 & 15) + 31744 | 0];
  HEAP8[$5 | 0] = HEAPU8[($7 >>> 4 | 0) + 31744 | 0];
  label$12 : {
   if (HEAPU8[$4 | 0] == 77) {
    HEAP8[$5 + 2 | 0] = $8;
    $4 = $5 + 3 | 0;
    break label$12;
   }
   $4 = $5 + 2 | 0;
  }
  $2 = HEAPU8[($9 ? 0 : 5) + $2 | 0];
  HEAP8[$4 + 2 | 0] = 0;
  HEAP8[$4 + 1 | 0] = HEAPU8[($2 & 15) + 31744 | 0];
  HEAP8[$4 | 0] = HEAPU8[($2 >>> 4 | 0) + 31744 | 0];
  $2 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  $3 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$6 + 8 >> 2] = $2;
  HEAP32[$6 + 12 >> 2] = $3;
  $0 = string($0, $1, $6 + 16 | 0, $6 + 8 | 0);
  global$0 = $6 + 48 | 0;
  return $0;
 }
 
 function resource_string($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $6 = global$0 - 176 | 0;
  global$0 = $6;
  HEAP8[$6 + 112 | 0] = 91;
  $9 = $6 + 112 | 1;
  $10 = HEAPU8[$4 | 0];
  $8 = $6 + 171 | 0;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        label$7 : {
         $7 = HEAP32[$2 + 12 >> 2];
         if (!($7 & 256)) {
          if ($7 & 512) {
           break label$7
          }
          $5 = 32549;
          $4 = 32484;
          if ($7 & 1024) {
           break label$5
          }
          $4 = 32489;
          if ($7 & 2048) {
           break label$5
          }
          if ($7 & 4096) {
           break label$6
          }
          $7 = 0;
          $4 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
          HEAP32[$6 + 80 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
          HEAP32[$6 + 84 >> 2] = $4;
          $4 = string($9, $8, 32499, $6 + 80 | 0);
          $5 = 32450;
          break label$4;
         }
         $5 = 32442;
         $4 = 32474;
         break label$5;
        }
        $5 = 32450;
        $4 = 32479;
        break label$5;
       }
       $5 = 32458;
       $4 = 32494;
      }
      $7 = 0;
      $11 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
      HEAP32[$6 + 104 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
      HEAP32[$6 + 108 >> 2] = $11;
      $4 = string($9, $8, $4, $6 + 104 | 0);
      if (($10 | 0) != 82) {
       break label$4
      }
      $7 = 1;
      if (HEAPU8[$2 + 15 | 0] & 32) {
       break label$3
      }
     }
     $9 = HEAP32[$2 >> 2];
     $10 = HEAPU8[$5 + 4 | 0] | HEAPU8[$5 + 5 | 0] << 8 | (HEAPU8[$5 + 6 | 0] << 16 | HEAPU8[$5 + 7 | 0] << 24);
     HEAP32[$6 + 72 >> 2] = HEAPU8[$5 | 0] | HEAPU8[$5 + 1 | 0] << 8 | (HEAPU8[$5 + 2 | 0] << 16 | HEAPU8[$5 + 3 | 0] << 24);
     HEAP32[$6 + 76 >> 2] = $10;
     $4 = number($4, $8, $9, 0, $6 + 72 | 0);
     if (HEAP32[$2 >> 2] != HEAP32[$2 + 4 >> 2]) {
      HEAP8[$4 | 0] = 45;
      $9 = HEAP32[$2 + 4 >> 2];
      $10 = HEAPU8[$5 + 4 | 0] | HEAPU8[$5 + 5 | 0] << 8 | (HEAPU8[$5 + 6 | 0] << 16 | HEAPU8[$5 + 7 | 0] << 24);
      HEAP32[$6 + 64 >> 2] = HEAPU8[$5 | 0] | HEAPU8[$5 + 1 | 0] << 8 | (HEAPU8[$5 + 2 | 0] << 16 | HEAPU8[$5 + 3 | 0] << 24);
      HEAP32[$6 + 68 >> 2] = $10;
      $4 = number($4 + 1 | 0, $8, $9, 0, $6 - -64 | 0);
     }
     $5 = $4;
     if ($7) {
      break label$2
     }
     $5 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
     HEAP32[$6 + 56 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
     HEAP32[$6 + 60 >> 2] = $5;
     $4 = string($4, $8, 32541, $6 + 56 | 0);
     $2 = HEAP32[$2 + 12 >> 2];
     $5 = HEAPU8[32561] | HEAPU8[32562] << 8 | (HEAPU8[32563] << 16 | HEAPU8[32564] << 24);
     HEAP32[$6 + 48 >> 2] = HEAPU8[32557] | HEAPU8[32558] << 8 | (HEAPU8[32559] << 16 | HEAPU8[32560] << 24);
     HEAP32[$6 + 52 >> 2] = $5;
     $4 = number($4, $8, $2, 0, $6 + 48 | 0);
     break label$1;
    }
    $7 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
    HEAP32[$6 + 96 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
    HEAP32[$6 + 100 >> 2] = $7;
    $4 = string($4, $8, 32504, $6 + 96 | 0);
    $7 = HEAP32[$2 >> 2];
    $9 = HEAP32[$2 + 4 >> 2];
    $10 = HEAPU8[$5 + 4 | 0] | HEAPU8[$5 + 5 | 0] << 8 | (HEAPU8[$5 + 6 | 0] << 16 | HEAPU8[$5 + 7 | 0] << 24);
    HEAP32[$6 + 88 >> 2] = HEAPU8[$5 | 0] | HEAPU8[$5 + 1 | 0] << 8 | (HEAPU8[$5 + 2 | 0] << 16 | HEAPU8[$5 + 3 | 0] << 24);
    HEAP32[$6 + 92 >> 2] = $10;
    $5 = number($4, $8, ($9 + 1 | 0) - $7 | 0, 0, $6 + 88 | 0);
   }
   $4 = $5;
   $7 = $2 + 12 | 0;
   $5 = HEAP32[$7 >> 2];
   if ($5 & 1048576) {
    $5 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
    HEAP32[$6 + 40 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
    HEAP32[$6 + 44 >> 2] = $5;
    $4 = string($4, $8, 32510, $6 + 40 | 0);
    $5 = HEAP32[$7 >> 2];
   }
   if ($5 & 8192) {
    $5 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
    HEAP32[$6 + 32 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
    HEAP32[$6 + 36 >> 2] = $5;
    $4 = string($4, $8, 32517, $6 + 32 | 0);
    $5 = HEAP32[$2 + 12 >> 2];
   }
   if ($5 & 2097152) {
    $5 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
    HEAP32[$6 + 24 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
    HEAP32[$6 + 28 >> 2] = $5;
    $4 = string($4, $8, 32523, $6 + 24 | 0);
    $5 = HEAP32[$2 + 12 >> 2];
   }
   if (!($5 & 268435456)) {
    break label$1
   }
   $2 = HEAPU8[32470] | HEAPU8[32471] << 8 | (HEAPU8[32472] << 16 | HEAPU8[32473] << 24);
   HEAP32[$6 + 16 >> 2] = HEAPU8[32466] | HEAPU8[32467] << 8 | (HEAPU8[32468] << 16 | HEAPU8[32469] << 24);
   HEAP32[$6 + 20 >> 2] = $2;
   $4 = string($4, $8, 32531, $6 + 16 | 0);
  }
  HEAP8[$4 | 0] = 93;
  HEAP8[$4 + 1 | 0] = 0;
  $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$6 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$6 + 12 >> 2] = $2;
  $0 = string($0, $1, $6 + 112 | 0, $6 + 8 | 0);
  global$0 = $6 + 176 | 0;
  return $0;
 }
 
 function clock($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $4 = HEAPU8[$2 | 0] | HEAPU8[$2 + 1 | 0] << 8 | (HEAPU8[$2 + 2 | 0] << 16 | HEAPU8[$2 + 3 | 0] << 24);
  $2 = HEAPU8[$2 + 4 | 0] | HEAPU8[$2 + 5 | 0] << 8 | (HEAPU8[$2 + 6 | 0] << 16 | HEAPU8[$2 + 7 | 0] << 24);
  HEAP32[$3 + 8 >> 2] = $4;
  HEAP32[$3 + 12 >> 2] = $2;
  $0 = string($0, $1, 0, $3 + 8 | 0);
  global$0 = $3 + 16 | 0;
  return $0;
 }
 
 function dentry_name($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $7 = global$0 - 32 | 0;
  global$0 = $7;
  label$1 : {
   label$2 : {
    $4 = HEAP8[$4 + 1 | 0];
    $10 = $4 + -50 >>> 0 < 3 ? $4 + -48 | 0 : 1;
    if (($10 | 0) >= 1) {
     $6 = $7 + 16 | 0;
     while (1) {
      $8 = HEAP32[$2 + 16 >> 2];
      $4 = HEAP32[$2 + 32 >> 2];
      HEAP32[$6 >> 2] = $4;
      if (($2 | 0) == ($8 | 0)) {
       break label$2
      }
      $6 = $6 + 4 | 0;
      $2 = $8;
      $5 = $5 + 1 | 0;
      if (($5 | 0) < ($10 | 0)) {
       continue
      }
      break;
     };
     break label$1;
    }
    break label$1;
   }
   if ($5) {
    HEAP32[$6 >> 2] = 32577;
    $4 = 32577;
   }
   $5 = $5 + 1 | 0;
  }
  $8 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  label$6 : {
   if ($8 >> 16) {
    $5 = $5 + -1 | 0;
    while (1) {
     $2 = $0 + $9 | 0;
     $6 = HEAPU8[$4 | 0];
     label$9 : {
      label$10 : {
       if (!$6) {
        if (!$5) {
         break label$6
        }
        $5 = $5 + -1 | 0;
        $4 = HEAP32[($7 + 16 | 0) + ($5 << 2) >> 2];
        $6 = 47;
        if ($2 >>> 0 < $1 >>> 0) {
         break label$10
        }
        break label$9;
       }
       $4 = $4 + 1 | 0;
       if ($2 >>> 0 >= $1 >>> 0) {
        break label$9
       }
      }
      HEAP8[$2 | 0] = $6;
      $8 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
     }
     $9 = $9 + 1 | 0;
     if (($9 | 0) != $8 >> 16) {
      continue
     }
     break;
    };
    $2 = $0 + $9 | 0;
    break label$6;
   }
   $2 = $0;
  }
  $0 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$7 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$7 + 12 >> 2] = $0;
  $0 = widen_string($2, $9, $1, $7 + 8 | 0);
  global$0 = $7 + 32 | 0;
  return $0;
 }
 
 function escaped_string($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  $6 = (HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) >> 8;
  label$1 : {
   if (!$6) {
    break label$1
   }
   if ($2 >>> 0 < 17) {
    $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
    HEAP32[$5 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
    HEAP32[$5 + 12 >> 2] = $2;
    $0 = string($0, $1, 0, $5 + 8 | 0);
    break label$1;
   }
   $3 = $4 + 1 | 0;
   while (1) {
    label$3 : {
     $8 = HEAP8[$3 | 0] + -97 | 0;
     if ($8 >>> 0 > 18) {
      break label$3
     }
     $4 = 15;
     label$5 : {
      label$6 : {
       switch ($8 - 1 | 0) {
       case 1:
        $4 = 2;
        break label$5;
       case 6:
        $4 = 32;
        break label$5;
       case 12:
        $4 = 4;
        break label$5;
       case 13:
        $4 = 8;
        break label$5;
       case 14:
        $4 = 16;
        break label$5;
       case 0:
       case 2:
       case 3:
       case 4:
       case 5:
       case 7:
       case 8:
       case 9:
       case 10:
       case 11:
       case 15:
       case 16:
        break label$3;
       case 17:
        break label$6;
       default:
        break label$5;
       };
      }
      $4 = 1;
     }
     $3 = $3 + 1 | 0;
     $7 = $4 | $7;
     continue;
    }
    break;
   };
   $0 = string_escape_mem($2, ($6 | 0) < 0 ? 1 : $6, $0, $0 >>> 0 < $1 >>> 0 ? $1 - $0 | 0 : 0, $7 ? $7 : 31) + $0 | 0;
  }
  global$0 = $5 + 16 | 0;
  return $0;
 }
 
 function flags_string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  label$1 : {
   label$2 : {
    label$3 : {
     $3 = HEAP8[$3 + 1 | 0];
     if (($3 | 0) != 103) {
      if (($3 | 0) == 118) {
       break label$3
      }
      if (($3 | 0) != 112) {
       break label$1
      }
      $3 = 24512;
      $2 = HEAP32[$2 >> 2] & 4194303;
      if (!$2) {
       break label$1
      }
      break label$2;
     }
     $3 = 25184;
     $2 = HEAP32[$2 >> 2];
     if ($2) {
      break label$2
     }
     break label$1;
    }
    $3 = 25728;
    $2 = HEAP32[$2 >> 2];
    if (!$2) {
     break label$1
    }
   }
   $5 = HEAP32[$3 + 4 >> 2];
   if ($5) {
    while (1) {
     $6 = HEAP32[$3 >> 2];
     if (($6 & $2) == ($6 | 0)) {
      $7 = HEAPU8[32582] | HEAPU8[32583] << 8 | (HEAPU8[32584] << 16 | HEAPU8[32585] << 24);
      HEAP32[$4 + 8 >> 2] = HEAPU8[32578] | HEAPU8[32579] << 8 | (HEAPU8[32580] << 16 | HEAPU8[32581] << 24);
      HEAP32[$4 + 12 >> 2] = $7;
      $0 = string($0, $1, $5, $4 + 8 | 0);
      $2 = ($6 ^ -1) & $2;
      if (!$2) {
       break label$1
      }
      if ($0 >>> 0 < $1 >>> 0) {
       HEAP8[$0 | 0] = 124
      }
      $0 = $0 + 1 | 0;
     }
     $5 = $3 + 12 | 0;
     $3 = $3 + 8 | 0;
     $5 = HEAP32[$5 >> 2];
     if ($5) {
      continue
     }
     break;
    }
   }
   $3 = HEAPU8[32561] | HEAPU8[32562] << 8 | (HEAPU8[32563] << 16 | HEAPU8[32564] << 24);
   HEAP32[$4 >> 2] = HEAPU8[32557] | HEAPU8[32558] << 8 | (HEAPU8[32559] << 16 | HEAPU8[32560] << 24);
   HEAP32[$4 + 4 >> 2] = $3;
   $0 = number($0, $1, $2, 0, $4);
  }
  global$0 = $4 + 16 | 0;
  return $0;
 }
 
 function restricted_pointer($0, $1, $2, $3) {
  var $4 = 0, $5 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  $4 = HEAP32[34968];
  label$1 : {
   label$2 : {
    if (!$4) {
     break label$2
    }
    if (($4 | 0) == 1) {
     label$4 : {
      if (HEAP32[1] & 983040) {
       break label$4
      }
      if (HEAP32[1] & 256) {
       break label$4
      }
      if (!(HEAP32[1] & 1048576)) {
       break label$2
      }
     }
     $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
     $4 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
     if ($4 >> 8 == -1) {
      $4 = $4 & 255 | 2048;
      HEAP8[$3 | 0] = $4;
      HEAP8[$3 + 1 | 0] = $4 >>> 8;
      HEAP8[$3 + 2 | 0] = $4 >>> 16;
      HEAP8[$3 + 3 | 0] = $4 >>> 24;
      HEAP8[$3 + 4 | 0] = $2;
      HEAP8[$3 + 5 | 0] = $2 >>> 8;
      HEAP8[$3 + 6 | 0] = $2 >>> 16;
      HEAP8[$3 + 7 | 0] = $2 >>> 24;
     }
     $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
     HEAP32[$5 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
     HEAP32[$5 + 12 >> 2] = $2;
     $3 = string($0, $1, 32568, $5 + 8 | 0);
     break label$1;
    }
    $2 = 0;
   }
   $4 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   HEAP32[$5 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
   HEAP32[$5 + 4 >> 2] = $4;
   $3 = pointer_string($0, $1, $2, $5);
  }
  global$0 = $5 + 16 | 0;
  return $3;
 }
 
 function netdev_bits($0, $1, $2, $3, $4) {
  var $5 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  label$1 : {
   if (HEAPU8[$4 + 1 | 0] == 70) {
    $0 = special_hex_number($0, $1, HEAP32[$2 >> 2], HEAP32[$2 + 4 >> 2], 8);
    break label$1;
   }
   $4 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   $3 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
   HEAP32[$5 + 24 >> 2] = $3;
   HEAP32[$5 + 28 >> 2] = $4;
   label$3 : {
    if (!HEAPU8[19920]) {
     if (HEAP32[4981] >= 1) {
      break label$3
     }
     $2 = siphash_1u32($2);
     $3 = HEAP32[$5 + 28 >> 2];
     HEAP32[$5 + 16 >> 2] = HEAP32[$5 + 24 >> 2];
     HEAP32[$5 + 20 >> 2] = $3;
     $0 = pointer_string($0, $1, $2, $5 + 16 | 0);
     break label$1;
    }
    $3 = HEAP32[$5 + 28 >> 2];
    HEAP32[$5 >> 2] = HEAP32[$5 + 24 >> 2];
    HEAP32[$5 + 4 >> 2] = $3;
    $0 = pointer_string($0, $1, Math_imul($2, 1640531527), $5);
    break label$1;
   }
   $2 = $3 & 255 | 2048;
   HEAP32[$5 + 8 >> 2] = $2;
   HEAP32[$5 + 12 >> 2] = $4;
   HEAP32[$5 + 24 >> 2] = $2;
   HEAP32[$5 + 28 >> 2] = $4;
   $0 = string($0, $1, 32592, $5 + 8 | 0);
  }
  global$0 = $5 + 32 | 0;
  return $0;
 }
 
 function device_node_string($0, $1, $2) {
  var $3 = 0, $4 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  $4 = HEAPU8[$2 | 0] | HEAPU8[$2 + 1 | 0] << 8 | (HEAPU8[$2 + 2 | 0] << 16 | HEAPU8[$2 + 3 | 0] << 24);
  $2 = HEAPU8[$2 + 4 | 0] | HEAPU8[$2 + 5 | 0] << 8 | (HEAPU8[$2 + 6 | 0] << 16 | HEAPU8[$2 + 7 | 0] << 24);
  HEAP32[$3 + 8 >> 2] = $4;
  HEAP32[$3 + 12 >> 2] = $2;
  $0 = string($0, $1, 32586, $3 + 8 | 0);
  global$0 = $3 + 16 | 0;
  return $0;
 }
 
 function pointer_string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $8 = (HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24)) & -65313 | 4128;
  $6 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  $5 = $6;
  $6 = $5 >> 8 == -1;
  $5 = $6 ? $5 & 255 | 2048 : $5;
  $4 = $5;
  HEAP8[$3 | 0] = $4;
  HEAP8[$3 + 1 | 0] = $4 >>> 8;
  HEAP8[$3 + 2 | 0] = $4 >>> 16;
  HEAP8[$3 + 3 | 0] = $4 >>> 24;
  $4 = $6 ? $8 & -61201 | 16 : $8;
  HEAP8[$3 + 4 | 0] = $4;
  HEAP8[$3 + 5 | 0] = $4 >>> 8;
  HEAP8[$3 + 6 | 0] = $4 >>> 16;
  HEAP8[$3 + 7 | 0] = $4 >>> 24;
  $3 = $7;
  HEAP32[$3 + 8 >> 2] = $5;
  HEAP32[$3 + 12 >> 2] = $4;
  $0 = number($0, $1, $2, 0, $3 + 8 | 0);
  global$0 = $3 + 16 | 0;
  return $0;
 }
 
 function uuid_string($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $6 = global$0 + -64 | 0;
  global$0 = $6;
  $8 = 31888;
  $5 = HEAP8[$4 + 1 | 0];
  label$1 : {
   if (($5 | 0) != 66) {
    $4 = 0;
    if (($5 | 0) != 108) {
     if (($5 | 0) != 76) {
      break label$1
     }
     $4 = 1;
    }
    $8 = 31872;
    break label$1;
   }
   $4 = 1;
  }
  $9 = $4 ? 31776 : 31744;
  $4 = 0;
  $5 = $6 + 16 | 0;
  while (1) {
   $7 = $5;
   $5 = HEAPU8[HEAPU8[$4 + $8 | 0] + $2 | 0];
   HEAP8[$7 + 1 | 0] = HEAPU8[($5 & 15) + $9 | 0];
   HEAP8[$7 | 0] = HEAPU8[($5 >>> 4 | 0) + $9 | 0];
   $5 = $7 + 2 | 0;
   label$5 : {
    if ($4 >>> 0 > 9) {
     break label$5
    }
    if (!(1 << $4 & 680)) {
     break label$5
    }
    HEAP8[$7 + 2 | 0] = 45;
    $5 = $7 + 3 | 0;
   }
   $4 = $4 + 1 | 0;
   if (($4 | 0) != 16) {
    continue
   }
   break;
  };
  HEAP8[$5 | 0] = 0;
  $2 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  $3 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$6 + 8 >> 2] = $2;
  HEAP32[$6 + 12 >> 2] = $3;
  $0 = string($0, $1, $6 + 16 | 0, $6 + 8 | 0);
  global$0 = $6 - -64 | 0;
  return $0;
 }
 
 function bitmap_list_string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  $3 = (HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) >> 8;
  $6 = ($3 | 0) > 0 ? $3 : 0;
  $8 = find_next_bit($2, $6, 0);
  $9 = 1;
  while (1) {
   $3 = $8;
   label$2 : {
    while (1) {
     $7 = $3;
     if (($3 | 0) >= ($6 | 0)) {
      break label$2
     }
     $5 = $7 + 1 | 0;
     $3 = find_next_bit($2, $6, $5);
     if (($3 | 0) <= ($5 | 0) ? ($3 | 0) < ($6 | 0) : 0) {
      continue
     }
     break;
    };
    if (!$9) {
     if ($0 >>> 0 < $1 >>> 0) {
      HEAP8[$0 | 0] = 44
     }
     $0 = $0 + 1 | 0;
    }
    $9 = 0;
    $5 = HEAPU8[32553] | HEAPU8[32554] << 8 | (HEAPU8[32555] << 16 | HEAPU8[32556] << 24);
    HEAP32[$4 + 8 >> 2] = HEAPU8[32549] | HEAPU8[32550] << 8 | (HEAPU8[32551] << 16 | HEAPU8[32552] << 24);
    HEAP32[$4 + 12 >> 2] = $5;
    $5 = $8;
    $0 = number($0, $1, $5, $5 >> 31, $4 + 8 | 0);
    $8 = $3;
    if (($5 | 0) >= ($7 | 0)) {
     continue
    }
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = 45
    }
    $3 = HEAPU8[32553] | HEAPU8[32554] << 8 | (HEAPU8[32555] << 16 | HEAPU8[32556] << 24);
    HEAP32[$4 >> 2] = HEAPU8[32549] | HEAPU8[32550] << 8 | (HEAPU8[32551] << 16 | HEAPU8[32552] << 24);
    HEAP32[$4 + 4 >> 2] = $3;
    $0 = number($0 + 1 | 0, $1, $7, $7 >> 31, $4);
    continue;
   }
   break;
  };
  global$0 = $4 + 16 | 0;
  return $0;
 }
 
 function hex_string($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0;
  $6 = global$0 - 16 | 0;
  global$0 = $6;
  $5 = (HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) >> 8;
  label$1 : {
   if (!$5) {
    break label$1
   }
   if ($2 >>> 0 <= 16) {
    $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
    HEAP32[$6 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
    HEAP32[$6 + 12 >> 2] = $2;
    $0 = string($0, $1, 0, $6 + 8 | 0);
    break label$1;
   }
   label$3 : {
    label$4 : {
     label$5 : {
      $3 = HEAP8[$4 + 1 | 0];
      if (($3 | 0) != 67) {
       if (($3 | 0) == 78) {
        break label$5
       }
       if (($3 | 0) != 68) {
        break label$4
       }
       $4 = 45;
       break label$3;
      }
      $4 = 58;
      break label$3;
     }
     $4 = 0;
     break label$3;
    }
    $4 = 32;
   }
   $3 = ($5 | 0) > 0 ? (($5 | 0) < 64 ? $5 : 64) : 1;
   if (($3 | 0) < 1) {
    break label$1
   }
   while (1) {
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = HEAPU8[(HEAPU8[$2 | 0] >>> 4 | 0) + 31744 | 0]
    }
    $5 = $0 + 1 | 0;
    if ($5 >>> 0 < $1 >>> 0) {
     HEAP8[$5 | 0] = HEAPU8[(HEAPU8[$2 | 0] & 15) + 31744 | 0]
    }
    $5 = $0 + 2 | 0;
    label$10 : {
     label$11 : {
      if (!$4) {
       break label$11
      }
      if (($3 | 0) == 1) {
       break label$11
      }
      if ($5 >>> 0 < $1 >>> 0) {
       HEAP8[$5 | 0] = $4
      }
      $0 = $0 + 3 | 0;
      break label$10;
     }
     $0 = $5;
    }
    $2 = $2 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  global$0 = $6 + 16 | 0;
  return $0;
 }
 
 function ip4_addr_string($0, $1, $2, $3, $4) {
  var $5 = 0;
  $5 = global$0 - 32 | 0;
  global$0 = $5;
  ip4_string($5 + 16 | 0, $2, $4);
  $2 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  $3 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$5 + 8 >> 2] = $2;
  HEAP32[$5 + 12 >> 2] = $3;
  $0 = string($0, $1, $5 + 16 | 0, $5 + 8 | 0);
  global$0 = $5 + 32 | 0;
  return $0;
 }
 
 function ip4_addr_string_sa($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $5 = global$0 + -64 | 0;
  global$0 = $5;
  HEAP8[$5 + 30 | 0] = 52;
  HEAP8[$5 + 31 | 0] = 0;
  HEAP8[$5 + 29 | 0] = HEAPU8[$4 | 0];
  $8 = $2 + 4 | 0;
  $7 = HEAPU8[$4 + 2 | 0];
  label$1 : {
   if (HEAPU8[$7 + 31952 | 0] & 3) {
    $4 = $4 + 3 | 0;
    $9 = $5 + 31 | 0;
    while (1) {
     $6 = $4;
     $4 = __wasm_rotl_i32(($7 << 24 >> 24) + -98 | 0, 31);
     label$4 : {
      if ($4 >>> 0 > 7) {
       break label$4
      }
      if (!(1 << $4 & 105)) {
       if (($4 | 0) != 7) {
        break label$4
       }
       $10 = 1;
       break label$4;
      }
      HEAP8[$9 | 0] = $7;
     }
     $4 = $6 + 1 | 0;
     $7 = HEAPU8[$6 | 0];
     if (HEAPU8[$7 + 31952 | 0] & 3) {
      continue
     }
     break;
    };
    $4 = ip4_string($5 + 32 | 0, $8, $5 + 29 | 0);
    $6 = $4;
    if (!$10) {
     break label$1
    }
    HEAP8[$4 | 0] = 58;
    $2 = HEAPU16[$2 + 2 >> 1];
    $6 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
    HEAP32[$5 + 16 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
    HEAP32[$5 + 20 >> 2] = $6;
    $6 = number($4 + 1 | 0, $5 + 55 | 0, ($2 << 8 & 16711680 | $2 << 24) >>> 16 | 0, 0, $5 + 16 | 0);
    break label$1;
   }
   $6 = ip4_string($5 + 32 | 0, $8, $5 + 29 | 0);
  }
  HEAP8[$6 | 0] = 0;
  $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$5 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$5 + 12 >> 2] = $2;
  $0 = string($0, $1, $5 + 32 | 0, $5 + 8 | 0);
  global$0 = $5 - -64 | 0;
  return $0;
 }
 
 function bitmap_string($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $7 = global$0 - 16 | 0;
  global$0 = $7;
  $4 = $3;
  $6 = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP8[$3 | 0] = 0;
  HEAP8[$3 + 1 | 0] = 0;
  HEAP8[$3 + 2 | 0] = 0;
  HEAP8[$3 + 3 | 0] = 0;
  HEAP8[$3 + 4 | 0] = 4144;
  HEAP8[$3 + 5 | 0] = 16;
  HEAP8[$3 + 6 | 0] = 0;
  HEAP8[$3 + 7 | 0] = 0;
  $5 = $6 >> 8;
  $5 = ($5 | 0) > 0 ? $5 : 0;
  $9 = $5 + 31 & 2147483616;
  $8 = $9 + -32 | 0;
  label$1 : {
   if (($8 | 0) < 0) {
    break label$1
   }
   $6 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   $10 = (HEAPU8[$4 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) & 255;
   $4 = $5 & 31;
   $5 = $4 ? $4 : 32;
   $4 = $10 | ($5 << 6) + 192 & 7936;
   HEAP8[$3 | 0] = $4;
   HEAP8[$3 + 1 | 0] = $4 >>> 8;
   HEAP8[$3 + 2 | 0] = $4 >>> 16;
   HEAP8[$3 + 3 | 0] = $4 >>> 24;
   HEAP8[$3 + 4 | 0] = $6;
   HEAP8[$3 + 5 | 0] = $6 >>> 8;
   HEAP8[$3 + 6 | 0] = $6 >>> 16;
   HEAP8[$3 + 7 | 0] = $6 >>> 24;
   $8 = HEAP32[($8 >>> 3 | 0) + $2 >> 2];
   HEAP32[$7 + 8 >> 2] = $4;
   HEAP32[$7 + 12 >> 2] = $6;
   $0 = number($0, $1, ((32 <= ($5 & 63) >>> 0 ? 0 : -1 << ($5 & 31)) ^ -1) & $8, 0, $7 + 8 | 0);
   $5 = $9 + -64 | 0;
   if (($5 | 0) < 0) {
    break label$1
   }
   while (1) {
    $9 = HEAP32[($5 >>> 3 | 0) + $2 >> 2];
    if ($0 >>> 0 < $1 >>> 0) {
     HEAP8[$0 | 0] = 44
    }
    $4 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
    $8 = (HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24)) & 255 | 2048;
    $6 = $8;
    HEAP8[$3 | 0] = $6;
    HEAP8[$3 + 1 | 0] = $6 >>> 8;
    HEAP8[$3 + 2 | 0] = $6 >>> 16;
    HEAP8[$3 + 3 | 0] = $6 >>> 24;
    HEAP8[$3 + 4 | 0] = $4;
    HEAP8[$3 + 5 | 0] = $4 >>> 8;
    HEAP8[$3 + 6 | 0] = $4 >>> 16;
    HEAP8[$3 + 7 | 0] = $4 >>> 24;
    HEAP32[$7 >> 2] = $6;
    HEAP32[$7 + 4 >> 2] = $4;
    $0 = number($0 + 1 | 0, $1, $9, 0, $7);
    $5 = $5 + -32 | 0;
    if (($5 | 0) > -1) {
     continue
    }
    break;
   };
  }
  global$0 = $7 + 16 | 0;
  return $0;
 }
 
 function ip6_addr_string_sa($0, $1, $2, $3, $4) {
  var $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0;
  $5 = global$0 - 128 | 0;
  global$0 = $5;
  HEAP8[$5 + 47 | 0] = 54;
  $12 = HEAPU8[$4 | 0];
  HEAP8[$5 + 46 | 0] = $12;
  $7 = HEAPU8[$4 + 2 | 0];
  if (HEAPU8[$7 + 31952 | 0] & 3) {
   $4 = $4 + 3 | 0;
   while (1) {
    $6 = $4;
    $4 = ($7 << 24 >> 24) + -99 | 0;
    label$3 : {
     if ($4 >>> 0 > 16) {
      break label$3
     }
     label$4 : {
      switch ($4 - 1 | 0) {
      default:
       $11 = 1;
       break label$3;
      case 2:
       $9 = 1;
       break label$3;
      case 12:
       $8 = 1;
       break label$3;
      case 0:
      case 1:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 13:
      case 14:
       break label$3;
      case 15:
       break label$4;
      };
     }
     $10 = 1;
    }
    $4 = $6 + 1 | 0;
    $7 = HEAPU8[$6 | 0];
    if (HEAPU8[$7 + 31952 | 0] & 3) {
     continue
    }
    break;
   };
  }
  $4 = $2 + 8 | 0;
  $7 = $8 | $10 | $9;
  if ($7 & 1) {
   HEAP8[$5 + 48 | 0] = 91;
   $6 = 1;
  } else {
   $6 = 0
  }
  $6 = $6 | $5 + 48;
  label$9 : {
   label$10 : {
    if (($12 | 0) != 73) {
     break label$10
    }
    if (($11 ^ 1) & 1) {
     break label$10
    }
    $4 = ip6_compressed_string($6, $4);
    break label$9;
   }
   $4 = ip6_string($6, $4, $5 + 46 | 0);
  }
  if ($7 & 1) {
   HEAP8[$4 | 0] = 93;
   $4 = $4 + 1 | 0;
  }
  $6 = $5 + 126 | 0;
  if ($8) {
   HEAP8[$4 | 0] = 58;
   $8 = HEAPU16[$2 + 2 >> 1];
   $11 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   HEAP32[$5 + 32 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
   HEAP32[$5 + 36 >> 2] = $11;
   $4 = number($4 + 1 | 0, $6, ($8 << 8 & 16711680 | $8 << 24) >>> 16 | 0, 0, $5 + 32 | 0);
  }
  if ($9) {
   HEAP8[$4 | 0] = 47;
   $9 = HEAP32[$2 + 4 >> 2];
   $8 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   HEAP32[$5 + 24 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
   HEAP32[$5 + 28 >> 2] = $8;
   $7 = $4 + 1 | 0;
   $4 = $9 & -241;
   $4 = number($7, $6, $4 << 24 | $4 << 8 & 16711680 | ($4 >>> 8 & 65280 | $4 >>> 24), 0, $5 + 24 | 0);
  }
  if ($10) {
   HEAP8[$4 | 0] = 37;
   $2 = HEAP32[$2 + 24 >> 2];
   $10 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
   HEAP32[$5 + 16 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
   HEAP32[$5 + 20 >> 2] = $10;
   $4 = number($4 + 1 | 0, $6, $2, 0, $5 + 16 | 0);
  }
  HEAP8[$4 | 0] = 0;
  $2 = HEAPU8[$3 + 4 | 0] | HEAPU8[$3 + 5 | 0] << 8 | (HEAPU8[$3 + 6 | 0] << 16 | HEAPU8[$3 + 7 | 0] << 24);
  HEAP32[$5 + 8 >> 2] = HEAPU8[$3 | 0] | HEAPU8[$3 + 1 | 0] << 8 | (HEAPU8[$3 + 2 | 0] << 16 | HEAPU8[$3 + 3 | 0] << 24);
  HEAP32[$5 + 12 >> 2] = $2;
  $0 = string($0, $1, $5 + 48 | 0, $5 + 8 | 0);
  global$0 = $5 + 128 | 0;
  return $0;
 }
 
 function vscnprintf($0, $1, $2, $3) {
  $0 = vsnprintf($0, $1, $2, $3);
  if ($0 >>> 0 < $1 >>> 0) {
   return $0
  }
  return $1 ? $1 + -1 | 0 : 0;
 }
 
 function snprintf($0, $1, $2, $3) {
  var $4 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  HEAP32[$4 + 12 >> 2] = $3;
  $0 = vsnprintf($0, $1, $2, $3);
  global$0 = $4 + 16 | 0;
  return $0;
 }
 
 function scnprintf($0, $1, $2, $3) {
  var $4 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  HEAP32[$4 + 12 >> 2] = $3;
  $3 = vsnprintf($0, $1, $2, $3);
  if ($3 >>> 0 >= $1 >>> 0) {
   $3 = $1 ? $1 + -1 | 0 : 0
  }
  global$0 = $4 + 16 | 0;
  return $3;
 }
 
 function sprintf($0, $1, $2) {
  var $3 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  HEAP32[$3 + 12 >> 2] = $2;
  $0 = vsnprintf($0, 2147483647, $1, $2);
  global$0 = $3 + 16 | 0;
  return $0;
 }
 
 function ip6_compressed_string($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  $3 = global$0 - 32 | 0;
  global$0 = $3;
  memcpy($3 + 8 | 0, $1, 16);
  $1 = HEAP32[$3 + 16 >> 2];
  label$1 : {
   label$2 : {
    if (HEAP32[$3 + 12 >> 2] | HEAP32[$3 + 8 >> 2] | $1 ^ -65536) {
     $7 = 8;
     memset($3 + 24 | 0, 0, 8);
     if (($1 | 2) == -27394046) {
      break label$2
     }
     break label$1;
    }
    memset($3 + 24 | 0, 0, 8);
   }
   $7 = 6;
   $10 = 1;
  }
  $2 = $3 + 8 | 0;
  while (1) {
   $4 = ($3 + 24 | 0) + $5 | 0;
   $1 = $2;
   $6 = $5;
   while (1) {
    if (!HEAPU16[$1 >> 1]) {
     HEAP8[$4 | 0] = HEAPU8[$4 | 0] + 1;
     $1 = $1 + 2 | 0;
     $6 = $6 + 1 | 0;
     if ($6 >>> 0 < $7 >>> 0) {
      continue
     }
    }
    break;
   };
   $2 = $2 + 2 | 0;
   $5 = $5 + 1 | 0;
   if (($5 | 0) != ($7 | 0)) {
    continue
   }
   break;
  };
  $6 = -1;
  $1 = 0;
  $2 = 1;
  while (1) {
   $5 = HEAPU8[($3 + 24 | 0) + $1 | 0];
   $4 = $2 >>> 0 < $5 >>> 0;
   $6 = $4 ? $1 : $6;
   $2 = $4 ? $5 : $2;
   $1 = $1 + 1 | 0;
   if (($7 | 0) != ($1 | 0)) {
    continue
   }
   break;
  };
  $8 = ($2 | 0) == 1 ? -1 : $6;
  $6 = ($8 + $2 | 0) + -1 | 0;
  $2 = 0;
  $1 = 0;
  while (1) {
   label$9 : {
    if (($1 | 0) == ($8 | 0)) {
     if (!(($2 ^ -1) & ($8 | 0) != 0)) {
      HEAP8[$0 | 0] = 58;
      $0 = $0 + 1 | 0;
     }
     HEAP8[$0 | 0] = 58;
     $0 = $0 + 1 | 0;
     $1 = $6;
     $2 = 0;
     break label$9;
    }
    if ($2 & 1) {
     HEAP8[$0 | 0] = 58;
     $0 = $0 + 1 | 0;
    }
    $2 = HEAPU16[($3 + 8 | 0) + ($1 << 1) >> 1];
    $2 = $2 << 24 | $2 << 8 & 16711680 | ($2 >>> 8 & 65280 | $2 >>> 24);
    $4 = $2 >>> 16 | 0;
    label$13 : {
     label$14 : {
      label$15 : {
       $2 = $2 >>> 24 | 0;
       if ($2) {
        if ($4 >>> 0 < 4096) {
         break label$15
        }
        HEAP8[$0 | 0] = HEAPU8[($2 >>> 4 | 0) + 31744 | 0];
        $5 = $2 & 15;
        $2 = 2;
        $9 = $0 + 1 | 0;
        break label$14;
       }
       $2 = $4 & 240;
       if (!$2) {
        HEAP8[$0 | 0] = HEAPU8[($4 & 15) + 31744 | 0];
        $0 = $0 + 1 | 0;
        $2 = 1;
        break label$9;
       }
       HEAP8[$0 + 1 | 0] = HEAPU8[($4 & 15) + 31744 | 0];
       HEAP8[$0 | 0] = HEAPU8[($2 >>> 4 | 0) + 31744 | 0];
       $0 = $0 + 2 | 0;
       break label$13;
      }
      $5 = $2 & 15;
      $2 = 1;
      $9 = $0;
     }
     HEAP8[$9 | 0] = HEAPU8[$5 + 31744 | 0];
     $0 = $0 + $2 | 0;
     HEAP8[$0 + 1 | 0] = HEAPU8[($4 & 15) + 31744 | 0];
     HEAP8[$0 | 0] = HEAPU8[(($4 & 240) >>> 4 | 0) + 31744 | 0];
     $0 = $0 + 2 | 0;
    }
    $2 = 1;
   }
   $1 = $1 + 1 | 0;
   if (($1 | 0) < ($7 | 0)) {
    continue
   }
   break;
  };
  if ($10) {
   if ($2) {
    HEAP8[$0 | 0] = 58;
    $0 = $0 + 1 | 0;
   }
   $0 = ip4_string($0, $3 + 20 | 0, 32565);
  }
  HEAP8[$0 | 0] = 0;
  global$0 = $3 + 32 | 0;
  return $0;
 }
 
 function ip6_string($0, $1, $2) {
  var $3 = 0;
  $3 = HEAPU8[$1 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 1 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$1 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$1;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 2 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 3 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$3 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$3;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 4 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 5 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$5 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$5;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 6 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 7 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$7 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$7;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 8 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 9 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$9 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$9;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 10 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 11 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$11 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $0 = $0 + 5 | 0;
    break label$11;
   }
   $0 = $0 + 4 | 0;
  }
  $3 = HEAPU8[$1 + 12 | 0];
  HEAP8[$0 + 1 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  $3 = HEAPU8[$1 + 13 | 0];
  HEAP8[$0 + 3 | 0] = HEAPU8[($3 & 15) + 31744 | 0];
  HEAP8[$0 + 2 | 0] = HEAPU8[($3 >>> 4 | 0) + 31744 | 0];
  label$13 : {
   if (HEAPU8[$2 | 0] == 73) {
    HEAP8[$0 + 4 | 0] = 58;
    $2 = $0 + 5 | 0;
    break label$13;
   }
   $2 = $0 + 4 | 0;
  }
  $0 = HEAPU8[$1 + 14 | 0];
  HEAP8[$2 + 1 | 0] = HEAPU8[($0 & 15) + 31744 | 0];
  HEAP8[$2 | 0] = HEAPU8[($0 >>> 4 | 0) + 31744 | 0];
  $0 = HEAPU8[$1 + 15 | 0];
  HEAP8[$2 + 4 | 0] = 0;
  HEAP8[$2 + 3 | 0] = HEAPU8[($0 & 15) + 31744 | 0];
  HEAP8[$2 + 2 | 0] = HEAPU8[($0 >>> 4 | 0) + 31744 | 0];
  return $2 + 4 | 0;
 }
 
 function ip4_string($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $4 = global$0 - 16 | 0;
  global$0 = $4;
  $3 = 3;
  $6 = HEAPU8[$2 | 0];
  $7 = -1;
  $2 = __wasm_rotl_i32(HEAP8[$2 + 2 | 0] + -98 | 0, 31);
  label$1 : {
   if (($2 | 0) == 3) {
    break label$1
   }
   if (($2 | 0) == 5) {
    break label$1
   }
   $7 = 1;
   $3 = 0;
  }
  $2 = put_dec_trunc8($4 + 12 | 0, HEAPU8[$1 + $3 | 0]) - ($4 + 12 | 0) | 0;
  label$2 : {
   label$3 : {
    label$4 : {
     label$5 : {
      if (($6 | 0) != 105) {
       break label$5
      }
      if (($2 | 0) > 2) {
       break label$5
      }
      HEAP8[$0 | 0] = 48;
      if (($2 | 0) > 1) {
       break label$4
      }
      HEAP8[$0 + 1 | 0] = 48;
      $0 = $0 + 2 | 0;
     }
     if ($2) {
      break label$3
     }
     break label$2;
    }
    $0 = $0 + 1 | 0;
   }
   $5 = $4 + 11 | 0;
   while (1) {
    HEAP8[$0 | 0] = HEAPU8[$2 + $5 | 0];
    $0 = $0 + 1 | 0;
    $2 = $2 + -1 | 0;
    if ($2) {
     continue
    }
    break;
   };
  }
  HEAP8[$0 | 0] = 46;
  $2 = $0 + 1 | 0;
  $5 = $3 + $7 | 0;
  $3 = put_dec_trunc8($4 + 12 | 0, HEAPU8[$5 + $1 | 0]) - ($4 + 12 | 0) | 0;
  label$7 : {
   label$8 : {
    label$9 : {
     label$10 : {
      if (($6 | 0) != 105) {
       break label$10
      }
      if (($3 | 0) > 2) {
       break label$10
      }
      HEAP8[$0 + 1 | 0] = 48;
      if (($3 | 0) > 1) {
       break label$9
      }
      HEAP8[$0 + 2 | 0] = 48;
      $2 = $0 + 3 | 0;
     }
     if ($3) {
      break label$8
     }
     break label$7;
    }
    $2 = $0 + 2 | 0;
   }
   $0 = $4 + 11 | 0;
   while (1) {
    HEAP8[$2 | 0] = HEAPU8[$0 + $3 | 0];
    $2 = $2 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  HEAP8[$2 | 0] = 46;
  $0 = $2 + 1 | 0;
  $5 = $5 + $7 | 0;
  $3 = put_dec_trunc8($4 + 12 | 0, HEAPU8[$5 + $1 | 0]) - ($4 + 12 | 0) | 0;
  label$12 : {
   label$13 : {
    label$14 : {
     label$15 : {
      if (($6 | 0) != 105) {
       break label$15
      }
      if (($3 | 0) > 2) {
       break label$15
      }
      HEAP8[$2 + 1 | 0] = 48;
      if (($3 | 0) > 1) {
       break label$14
      }
      HEAP8[$2 + 2 | 0] = 48;
      $0 = $2 + 3 | 0;
     }
     if ($3) {
      break label$13
     }
     break label$12;
    }
    $0 = $2 + 2 | 0;
   }
   $2 = $4 + 11 | 0;
   while (1) {
    HEAP8[$0 | 0] = HEAPU8[$2 + $3 | 0];
    $0 = $0 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  HEAP8[$0 | 0] = 46;
  $2 = $0 + 1 | 0;
  $3 = put_dec_trunc8($4 + 12 | 0, HEAPU8[($5 + $7 | 0) + $1 | 0]) - ($4 + 12 | 0) | 0;
  label$17 : {
   label$18 : {
    label$19 : {
     label$20 : {
      if (($6 | 0) != 105) {
       break label$20
      }
      if (($3 | 0) > 2) {
       break label$20
      }
      HEAP8[$0 + 1 | 0] = 48;
      if (($3 | 0) > 1) {
       break label$19
      }
      HEAP8[$0 + 2 | 0] = 48;
      $2 = $0 + 3 | 0;
     }
     if ($3) {
      break label$18
     }
     break label$17;
    }
    $2 = $0 + 2 | 0;
   }
   $0 = $4 + 11 | 0;
   while (1) {
    HEAP8[$2 | 0] = HEAPU8[$0 + $3 | 0];
    $2 = $2 + 1 | 0;
    $3 = $3 + -1 | 0;
    if ($3) {
     continue
    }
    break;
   };
  }
  HEAP8[$2 | 0] = 0;
  global$0 = $4 + 16 | 0;
  return $2;
 }
 
 function special_hex_number($0, $1, $2, $3, $4) {
  var $5 = 0;
  $5 = global$0 - 16 | 0;
  global$0 = $5;
  $4 = ($4 << 9) + 512 | 5;
  HEAP32[$5 >> 2] = $4;
  HEAP32[$5 + 4 >> 2] = -61328;
  HEAP32[$5 + 8 >> 2] = $4;
  HEAP32[$5 + 12 >> 2] = -61328;
  $0 = number($0, $1, $2, $3, $5);
  global$0 = $5 + 16 | 0;
  return $0;
 }
 
 function next_arg($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0;
  $7 = HEAPU8[$0 | 0];
  $8 = ($7 | 0) == 34;
  $3 = $8 ? $0 + 1 | 0 : $0;
  $5 = HEAPU8[$0 + $8 | 0];
  label$1 : {
   label$2 : {
    if ($5) {
     label$4 : {
      if (($7 | 0) != 34) {
       $0 = 0;
       $4 = $3;
       if (HEAPU8[$5 + 31952 | 0] & 32) {
        break label$4
       }
      }
      $9 = ($7 | 0) == 34;
      $10 = 1;
      $0 = 0;
      while (1) {
       $6 = $10;
       $11 = $5;
       $0 = $0 ? $0 : ($5 | 0) == 61 ? $6 + -1 | 0 : 0;
       $4 = $3 + $6 | 0;
       $5 = HEAPU8[$4 | 0];
       if (!$5) {
        break label$4
       }
       $10 = $6 + 1 | 0;
       $9 = ($11 | 0) == 34 ? ($8 ^ -1) & 1 : $9;
       $8 = ($9 | 0) != 0;
       if ($9) {
        continue
       }
       if (!(HEAPU8[$5 + 31952 | 0] & 32)) {
        continue
       }
       break;
      };
     }
     HEAP32[$1 >> 2] = $3;
     if (!$0) {
      break label$2
     }
     $1 = $0 + $3 | 0;
     HEAP8[$1 | 0] = 0;
     $0 = $1 + 1 | 0;
     HEAP32[$2 >> 2] = $0;
     if (HEAPU8[$1 + 1 | 0] != 34) {
      break label$1
     }
     HEAP32[$2 >> 2] = $0 + 1;
     $0 = ($3 + $6 | 0) + -1 | 0;
     if (HEAPU8[$0 | 0] != 34) {
      break label$1
     }
     HEAP8[$0 | 0] = 0;
     break label$1;
    }
    HEAP32[$1 >> 2] = $3;
    $4 = $3;
   }
   HEAP32[$2 >> 2] = 0;
  }
  label$7 : {
   if (($7 | 0) != 34) {
    break label$7
   }
   $0 = ($3 + $6 | 0) + -1 | 0;
   if (HEAPU8[$0 | 0] != 34) {
    break label$7
   }
   HEAP8[$0 | 0] = 0;
  }
  if (HEAPU8[$4 | 0]) {
   HEAP8[$4 | 0] = 0;
   $4 = $4 + 1 | 0;
  }
  return skip_spaces($4);
 }
 
 function rb_insert_color($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0;
  folding_inner0 : {
   $2 = HEAP32[$0 >> 2];
   label$1 : {
    if ($2) {
     label$4 : {
      label$5 : {
       label$6 : {
        label$7 : {
         label$8 : {
          label$9 : {
           label$10 : {
            while (1) {
             $4 = HEAP32[$2 >> 2];
             if ($4 & 1) {
              break label$4
             }
             label$12 : {
              $5 = $4;
              $3 = HEAP32[$4 + 4 >> 2];
              label$13 : {
               if (($2 | 0) != ($3 | 0)) {
                if (!$3) {
                 break label$12
                }
                if (HEAP8[$3 | 0] & 1) {
                 break label$12
                }
                $0 = $4 | 1;
                HEAP32[$3 >> 2] = $0;
                HEAP32[$2 >> 2] = $0;
                $2 = HEAP32[$4 >> 2] & -4;
                break label$13;
               }
               $3 = HEAP32[$4 + 8 >> 2];
               if (!$3) {
                break label$10
               }
               if (HEAP8[$3 | 0] & 1) {
                break label$10
               }
               $0 = $4 | 1;
               HEAP32[$3 >> 2] = $0;
               HEAP32[$2 >> 2] = $0;
               $2 = HEAP32[$4 >> 2] & -4;
              }
              HEAP32[$5 >> 2] = $2;
              $0 = $4;
              if ($2) {
               continue
              }
              break label$1;
             }
             break;
            };
            $3 = HEAP32[$2 + 4 >> 2];
            if (($3 | 0) == ($0 | 0)) {
             break label$9
            }
            $0 = $2;
            break label$8;
           }
           $3 = HEAP32[$2 + 8 >> 2];
           if (($3 | 0) == ($0 | 0)) {
            break label$7
           }
           $0 = $2;
           break label$6;
          }
          $3 = HEAP32[$0 + 8 >> 2];
          HEAP32[$2 + 4 >> 2] = $3;
          HEAP32[$0 + 8 >> 2] = $2;
          if ($3) {
           HEAP32[$3 >> 2] = $2 | 1
          }
          HEAP32[$2 >> 2] = $0;
          $3 = HEAP32[$0 + 4 >> 2];
         }
         HEAP32[$4 + 8 >> 2] = $3;
         HEAP32[$0 + 4 >> 2] = $4;
         if ($3) {
          HEAP32[$3 >> 2] = $4 | 1
         }
         $2 = HEAP32[$4 >> 2];
         HEAP32[$0 >> 2] = $2;
         HEAP32[$4 >> 2] = $0;
         $2 = $2 & -4;
         if (!$2) {
          break label$5
         }
         if (($4 | 0) != HEAP32[$2 + 8 >> 2]) {
          HEAP32[$2 + 4 >> 2] = $0;
          return;
         }
         break folding_inner0;
        }
        $3 = HEAP32[$0 + 4 >> 2];
        HEAP32[$2 + 8 >> 2] = $3;
        HEAP32[$0 + 4 >> 2] = $2;
        if ($3) {
         HEAP32[$3 >> 2] = $2 | 1
        }
        HEAP32[$2 >> 2] = $0;
        $3 = HEAP32[$0 + 8 >> 2];
       }
       HEAP32[$4 + 4 >> 2] = $3;
       HEAP32[$0 + 8 >> 2] = $4;
       if ($3) {
        HEAP32[$3 >> 2] = $4 | 1
       }
       $2 = HEAP32[$4 >> 2];
       HEAP32[$0 >> 2] = $2;
       HEAP32[$4 >> 2] = $0;
       $2 = $2 & -4;
       if (!$2) {
        break label$5
       }
       if (HEAP32[$2 + 8 >> 2] == ($4 | 0)) {
        break folding_inner0
       }
       HEAP32[$2 + 4 >> 2] = $0;
       return;
      }
      HEAP32[$1 >> 2] = $0;
     }
     return;
    }
    $4 = $0;
   }
   HEAP32[$4 >> 2] = 1;
   return;
  }
  HEAP32[$2 + 8 >> 2] = $0;
 }
 
 function rb_erase($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $5 = HEAP32[$0 + 4 >> 2];
  folding_inner0 : {
   label$1 : {
    label$2 : {
     label$3 : {
      label$4 : {
       label$5 : {
        label$6 : {
         label$7 : {
          label$8 : {
           label$9 : {
            label$10 : {
             label$11 : {
              label$12 : {
               label$13 : {
                label$14 : {
                 label$15 : {
                  $6 = HEAP32[$0 + 8 >> 2];
                  if ($6) {
                   if (!$5) {
                    break label$15
                   }
                   $2 = HEAP32[$5 + 8 >> 2];
                   if (!$2) {
                    break label$14
                   }
                   $3 = $5;
                   while (1) {
                    $4 = $3;
                    $7 = $2;
                    $3 = $2;
                    $2 = HEAP32[$2 + 8 >> 2];
                    if ($2) {
                     continue
                    }
                    break;
                   };
                   $2 = HEAP32[$7 + 4 >> 2];
                   HEAP32[$4 + 8 >> 2] = $2;
                   HEAP32[$7 + 4 >> 2] = $5;
                   HEAP32[$5 >> 2] = $7 | HEAP32[$5 >> 2] & 1;
                   $5 = $7;
                   break label$13;
                  }
                  $2 = HEAP32[$0 >> 2];
                  $4 = $2 & -4;
                  if (!$4) {
                   break label$12
                  }
                  if (HEAP32[$4 + 8 >> 2] == ($0 | 0)) {
                   break label$10
                  }
                  HEAP32[$4 + 4 >> 2] = $5;
                  if (!$5) {
                   break label$7
                  }
                  break label$1;
                 }
                 $2 = HEAP32[$0 >> 2];
                 HEAP32[$6 >> 2] = $2;
                 $2 = $2 & -4;
                 if (!$2) {
                  break label$9
                 }
                 if (HEAP32[$2 + 8 >> 2] == ($0 | 0)) {
                  break label$8
                 }
                 HEAP32[$2 + 4 >> 2] = $6;
                 return;
                }
                $2 = HEAP32[$5 + 4 >> 2];
                $4 = $5;
               }
               HEAP32[$6 >> 2] = HEAP32[$6 >> 2] & 1 | $5;
               HEAP32[$5 + 8 >> 2] = $6;
               $7 = HEAP32[$0 >> 2];
               $3 = $7 & -4;
               if ($3) {
                if (HEAP32[$3 + 8 >> 2] == ($0 | 0)) {
                 break label$11
                }
                HEAP32[$3 + 4 >> 2] = $5;
                if (!$2) {
                 break label$6
                }
                break label$2;
               }
               HEAP32[$1 >> 2] = $5;
               if ($2) {
                break label$2
               }
               break label$6;
              }
              HEAP32[$1 >> 2] = $5;
              if ($5) {
               break label$1
              }
              break label$7;
             }
             HEAP32[$3 + 8 >> 2] = $5;
             if ($2) {
              break label$2
             }
             break label$6;
            }
            HEAP32[$4 + 8 >> 2] = $5;
            if ($5) {
             break label$1
            }
            break label$7;
           }
           HEAP32[$1 >> 2] = $6;
           return;
          }
          HEAP32[$2 + 8 >> 2] = $6;
          return;
         }
         if (!($2 & 1)) {
          break label$4
         }
         if (!$4) {
          break label$4
         }
         break label$5;
        }
        $0 = HEAP32[$5 >> 2];
        HEAP32[$5 >> 2] = $7;
        if (!($0 & 1)) {
         break label$4
        }
       }
       $2 = 0;
       label$19 : {
        label$20 : {
         label$21 : {
          label$22 : {
           label$23 : {
            label$24 : {
             label$26 : {
              label$27 : {
               label$28 : {
                while (1) {
                 label$29 : {
                  label$31 : {
                   label$32 : {
                    label$33 : {
                     label$34 : {
                      label$35 : {
                       label$36 : {
                        label$37 : {
                         label$38 : {
                          label$39 : {
                           label$40 : {
                            label$41 : {
                             label$42 : {
                              label$43 : {
                               $0 = HEAP32[$4 + 4 >> 2];
                               if (($2 | 0) != ($0 | 0)) {
                                if (HEAP8[$0 | 0] & 1) {
                                 break label$43
                                }
                                $2 = HEAP32[$0 + 8 >> 2];
                                HEAP32[$2 >> 2] = $4 | 1;
                                HEAP32[$4 + 4 >> 2] = $2;
                                $3 = HEAP32[$4 >> 2];
                                HEAP32[$0 >> 2] = $3;
                                HEAP32[$0 + 8 >> 2] = $4;
                                HEAP32[$4 >> 2] = $0;
                                $3 = $3 & -4;
                                if (!$3) {
                                 break label$41
                                }
                                if (HEAP32[$3 + 8 >> 2] == ($4 | 0)) {
                                 break label$40
                                }
                                $3 = $3 + 4 | 0;
                                break label$39;
                               }
                               $2 = HEAP32[$4 + 8 >> 2];
                               if (HEAP8[$2 | 0] & 1) {
                                break label$42
                               }
                               $0 = HEAP32[$2 + 4 >> 2];
                               HEAP32[$0 >> 2] = $4 | 1;
                               HEAP32[$4 + 8 >> 2] = $0;
                               $3 = HEAP32[$4 >> 2];
                               HEAP32[$2 >> 2] = $3;
                               HEAP32[$2 + 4 >> 2] = $4;
                               HEAP32[$4 >> 2] = $2;
                               $3 = $3 & -4;
                               if (!$3) {
                                break label$36
                               }
                               if (HEAP32[$3 + 8 >> 2] == ($4 | 0)) {
                                break label$35
                               }
                               $3 = $3 + 4 | 0;
                               break label$34;
                              }
                              $2 = $0;
                              $3 = HEAP32[$2 + 4 >> 2];
                              if ($3) {
                               break label$38
                              }
                              break label$37;
                             }
                             $0 = $2;
                             $3 = HEAP32[$2 + 8 >> 2];
                             if ($3) {
                              break label$33
                             }
                             break label$32;
                            }
                            $3 = $1;
                            break label$39;
                           }
                           $3 = $3 + 8 | 0;
                          }
                          HEAP32[$3 >> 2] = $0;
                          $3 = HEAP32[$2 + 4 >> 2];
                          if (!$3) {
                           break label$37
                          }
                         }
                         if (!(HEAP8[$3 | 0] & 1)) {
                          break label$28
                         }
                        }
                        $0 = HEAP32[$2 + 8 >> 2];
                        if (HEAP8[$0 | 0] & 1 ? 0 : $0) {
                         break label$27
                        }
                        HEAP32[$2 >> 2] = $4;
                        $0 = HEAP32[$4 >> 2];
                        if (!($0 & 1)) {
                         break label$29
                        }
                        $0 = $0 & -4;
                        if ($0) {
                         break label$31
                        }
                        break label$4;
                       }
                       $3 = $1;
                       break label$34;
                      }
                      $3 = $3 + 8 | 0;
                     }
                     HEAP32[$3 >> 2] = $2;
                     $3 = HEAP32[$0 + 8 >> 2];
                     if (!$3) {
                      break label$32
                     }
                    }
                    if (!(HEAP8[$3 | 0] & 1)) {
                     break label$23
                    }
                   }
                   $2 = HEAP32[$0 + 4 >> 2];
                   if (HEAP8[$2 | 0] & 1 ? 0 : $2) {
                    break label$22
                   }
                   HEAP32[$0 >> 2] = $4;
                   $0 = HEAP32[$4 >> 2];
                   if (!($0 & 1)) {
                    break folding_inner0
                   }
                   $0 = $0 & -4;
                   if (!$0) {
                    break label$4
                   }
                  }
                  $2 = $4;
                  $4 = $0;
                  continue;
                 }
                 break;
                };
                break folding_inner0;
               }
               $0 = $2;
               $2 = $3;
               break label$26;
              }
              $3 = HEAP32[$0 + 4 >> 2];
              HEAP32[$2 + 8 >> 2] = $3;
              HEAP32[$0 + 4 >> 2] = $2;
              HEAP32[$4 + 4 >> 2] = $0;
              if (!$3) {
               break label$26
              }
              HEAP32[$3 >> 2] = $2 | 1;
             }
             $3 = HEAP32[$0 + 8 >> 2];
             HEAP32[$4 + 4 >> 2] = $3;
             $5 = $2;
             $2 = $0 | 1;
             HEAP32[$5 >> 2] = $2;
             HEAP32[$0 + 8 >> 2] = $4;
             if ($3) {
              HEAP32[$3 >> 2] = HEAP32[$3 >> 2] & 1 | $4
             }
             $3 = HEAP32[$4 >> 2];
             HEAP32[$0 >> 2] = $3;
             HEAP32[$4 >> 2] = $2;
             $2 = $3 & -4;
             if (!$2) {
              break label$24
             }
             if (HEAP32[$2 + 8 >> 2] == ($4 | 0)) {
              break label$20
             }
             HEAP32[$2 + 4 >> 2] = $0;
             return;
            }
            HEAP32[$1 >> 2] = $0;
            return;
           }
           $2 = $0;
           $0 = $3;
           break label$21;
          }
          $3 = HEAP32[$2 + 8 >> 2];
          HEAP32[$0 + 4 >> 2] = $3;
          HEAP32[$2 + 8 >> 2] = $0;
          HEAP32[$4 + 8 >> 2] = $2;
          if (!$3) {
           break label$21
          }
          HEAP32[$3 >> 2] = $0 | 1;
         }
         $3 = HEAP32[$2 + 4 >> 2];
         HEAP32[$4 + 8 >> 2] = $3;
         $5 = $0;
         $0 = $2 | 1;
         HEAP32[$5 >> 2] = $0;
         HEAP32[$2 + 4 >> 2] = $4;
         if ($3) {
          HEAP32[$3 >> 2] = HEAP32[$3 >> 2] & 1 | $4
         }
         $3 = HEAP32[$4 >> 2];
         HEAP32[$2 >> 2] = $3;
         HEAP32[$4 >> 2] = $0;
         $0 = $3 & -4;
         if (!$0) {
          break label$19
         }
         if (HEAP32[$0 + 8 >> 2] == ($4 | 0)) {
          break label$3
         }
         HEAP32[$0 + 4 >> 2] = $2;
         return;
        }
        HEAP32[$2 + 8 >> 2] = $0;
        return;
       }
       HEAP32[$1 >> 2] = $2;
      }
      return;
     }
     HEAP32[$0 + 8 >> 2] = $2;
     return;
    }
    HEAP32[$5 >> 2] = $7;
    HEAP32[$2 >> 2] = $4 | 1;
    return;
   }
   HEAP32[$5 >> 2] = $2;
   return;
  }
  HEAP32[$4 >> 2] = $0 | 1;
 }
 
 function rb_insert_color_cached($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0;
  $3 = HEAP32[$0 >> 2];
  if ($2) {
   HEAP32[$1 + 4 >> 2] = $0
  }
  folding_inner0 : {
   label$2 : {
    if ($3) {
     label$5 : {
      label$6 : {
       label$7 : {
        label$8 : {
         label$9 : {
          label$10 : {
           label$11 : {
            while (1) {
             $2 = HEAP32[$3 >> 2];
             if ($2 & 1) {
              break label$5
             }
             label$13 : {
              $5 = $2;
              $4 = HEAP32[$2 + 4 >> 2];
              label$14 : {
               if (($3 | 0) != ($4 | 0)) {
                if (!$4) {
                 break label$13
                }
                if (HEAP8[$4 | 0] & 1) {
                 break label$13
                }
                $0 = $2 | 1;
                HEAP32[$4 >> 2] = $0;
                HEAP32[$3 >> 2] = $0;
                $3 = HEAP32[$2 >> 2] & -4;
                break label$14;
               }
               $4 = HEAP32[$2 + 8 >> 2];
               if (!$4) {
                break label$11
               }
               if (HEAP8[$4 | 0] & 1) {
                break label$11
               }
               $0 = $2 | 1;
               HEAP32[$4 >> 2] = $0;
               HEAP32[$3 >> 2] = $0;
               $3 = HEAP32[$2 >> 2] & -4;
              }
              HEAP32[$5 >> 2] = $3;
              $0 = $2;
              if ($3) {
               continue
              }
              break label$2;
             }
             break;
            };
            $4 = HEAP32[$3 + 4 >> 2];
            if (($4 | 0) == ($0 | 0)) {
             break label$10
            }
            $0 = $3;
            break label$9;
           }
           $4 = HEAP32[$3 + 8 >> 2];
           if (($4 | 0) == ($0 | 0)) {
            break label$8
           }
           $0 = $3;
           break label$7;
          }
          $4 = HEAP32[$0 + 8 >> 2];
          HEAP32[$3 + 4 >> 2] = $4;
          HEAP32[$0 + 8 >> 2] = $3;
          if ($4) {
           HEAP32[$4 >> 2] = $3 | 1
          }
          HEAP32[$3 >> 2] = $0;
          $4 = HEAP32[$0 + 4 >> 2];
         }
         HEAP32[$2 + 8 >> 2] = $4;
         HEAP32[$0 + 4 >> 2] = $2;
         if ($4) {
          HEAP32[$4 >> 2] = $2 | 1
         }
         $3 = HEAP32[$2 >> 2];
         HEAP32[$0 >> 2] = $3;
         HEAP32[$2 >> 2] = $0;
         $3 = $3 & -4;
         if (!$3) {
          break label$6
         }
         if (($2 | 0) != HEAP32[$3 + 8 >> 2]) {
          HEAP32[$3 + 4 >> 2] = $0;
          return;
         }
         break folding_inner0;
        }
        $4 = HEAP32[$0 + 4 >> 2];
        HEAP32[$3 + 8 >> 2] = $4;
        HEAP32[$0 + 4 >> 2] = $3;
        if ($4) {
         HEAP32[$4 >> 2] = $3 | 1
        }
        HEAP32[$3 >> 2] = $0;
        $4 = HEAP32[$0 + 8 >> 2];
       }
       HEAP32[$2 + 4 >> 2] = $4;
       HEAP32[$0 + 8 >> 2] = $2;
       if ($4) {
        HEAP32[$4 >> 2] = $2 | 1
       }
       $3 = HEAP32[$2 >> 2];
       HEAP32[$0 >> 2] = $3;
       HEAP32[$2 >> 2] = $0;
       $3 = $3 & -4;
       if (!$3) {
        break label$6
       }
       if (HEAP32[$3 + 8 >> 2] == ($2 | 0)) {
        break folding_inner0
       }
       HEAP32[$3 + 4 >> 2] = $0;
       return;
      }
      HEAP32[$1 >> 2] = $0;
     }
     return;
    }
    $2 = $0;
   }
   HEAP32[$2 >> 2] = 1;
   return;
  }
  HEAP32[$3 + 8 >> 2] = $0;
 }
 
 function rb_erase_cached($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $4 = HEAP32[$0 + 8 >> 2];
  $6 = HEAP32[$0 + 4 >> 2];
  folding_inner0 : {
   label$1 : {
    label$2 : {
     label$3 : {
      label$4 : {
       label$5 : {
        label$6 : {
         if (($0 | 0) != HEAP32[$1 + 4 >> 2]) {
          if (!$4) {
           break label$6
          }
          break label$5;
         }
         $3 = HEAP32[$0 >> 2];
         label$8 : {
          if (($3 | 0) == ($0 | 0)) {
           break label$8
          }
          if ($6) {
           $3 = $6;
           while (1) {
            $2 = $3;
            $3 = HEAP32[$3 + 8 >> 2];
            if ($3) {
             continue
            }
            break;
           };
           break label$8;
          }
          label$11 : {
           $3 = $3 & -4;
           if ($3) {
            $5 = $0;
            while (1) {
             if (HEAP32[$3 + 4 >> 2] != ($5 | 0)) {
              break label$11
             }
             $5 = $3;
             $3 = HEAP32[$3 >> 2] & -4;
             if ($3) {
              continue
             }
             break;
            };
            break label$8;
           }
           $2 = $3;
           break label$8;
          }
          $2 = $3;
         }
         HEAP32[$1 + 4 >> 2] = $2;
         if ($4) {
          break label$5
         }
        }
        label$14 : {
         label$15 : {
          label$16 : {
           $2 = HEAP32[$0 >> 2];
           $3 = $2 & -4;
           if ($3) {
            if (HEAP32[$3 + 8 >> 2] == ($0 | 0)) {
             break label$16
            }
            HEAP32[$3 + 4 >> 2] = $6;
            if (!$6) {
             break label$15
            }
            break label$14;
           }
           HEAP32[$1 >> 2] = $6;
           if ($6) {
            break label$14
           }
           break label$15;
          }
          HEAP32[$3 + 8 >> 2] = $6;
          if ($6) {
           break label$14
          }
         }
         if (!($2 & 1)) {
          break label$3
         }
         if (!$3) {
          break label$3
         }
         break label$4;
        }
        HEAP32[$6 >> 2] = $2;
        return;
       }
       label$18 : {
        label$19 : {
         label$20 : {
          label$21 : {
           label$22 : {
            if ($6) {
             $4 = HEAP32[$6 + 8 >> 2];
             if (!$4) {
              break label$22
             }
             $5 = $6;
             while (1) {
              $3 = $5;
              $2 = $4;
              $5 = $2;
              $4 = HEAP32[$2 + 8 >> 2];
              if ($4) {
               continue
              }
              break;
             };
             $4 = HEAP32[$2 + 4 >> 2];
             HEAP32[$3 + 8 >> 2] = $4;
             HEAP32[$2 + 4 >> 2] = $6;
             HEAP32[$6 >> 2] = $2 | HEAP32[$6 >> 2] & 1;
             $6 = $2;
             break label$21;
            }
            $3 = HEAP32[$0 >> 2];
            HEAP32[$4 >> 2] = $3;
            $3 = $3 & -4;
            if (!$3) {
             break label$20
            }
            if (HEAP32[$3 + 8 >> 2] == ($0 | 0)) {
             break label$19
            }
            HEAP32[$3 + 4 >> 2] = $4;
            return;
           }
           $4 = HEAP32[$6 + 4 >> 2];
           $3 = $6;
          }
          $2 = HEAP32[$0 + 8 >> 2];
          HEAP32[$2 >> 2] = HEAP32[$2 >> 2] & 1 | $6;
          HEAP32[$6 + 8 >> 2] = $2;
          label$25 : {
           $5 = HEAP32[$0 >> 2];
           $2 = $5 & -4;
           if ($2) {
            if (HEAP32[$2 + 8 >> 2] == ($0 | 0)) {
             break label$25
            }
            HEAP32[$2 + 4 >> 2] = $6;
            if (!$4) {
             break label$18
            }
            break label$1;
           }
           HEAP32[$1 >> 2] = $6;
           if ($4) {
            break label$1
           }
           break label$18;
          }
          HEAP32[$2 + 8 >> 2] = $6;
          if ($4) {
           break label$1
          }
          break label$18;
         }
         HEAP32[$1 >> 2] = $4;
         return;
        }
        HEAP32[$3 + 8 >> 2] = $4;
        return;
       }
       $0 = HEAP32[$6 >> 2];
       HEAP32[$6 >> 2] = $5;
       if (!($0 & 1)) {
        break label$3
       }
      }
      $4 = 0;
      label$27 : {
       label$28 : {
        label$29 : {
         label$30 : {
          label$31 : {
           label$32 : {
            label$34 : {
             label$35 : {
              label$36 : {
               while (1) {
                label$37 : {
                 label$39 : {
                  label$40 : {
                   label$41 : {
                    label$42 : {
                     label$43 : {
                      label$44 : {
                       label$45 : {
                        label$46 : {
                         label$47 : {
                          label$48 : {
                           label$49 : {
                            label$50 : {
                             label$51 : {
                              $0 = HEAP32[$3 + 4 >> 2];
                              if (($4 | 0) != ($0 | 0)) {
                               if (HEAP8[$0 | 0] & 1) {
                                break label$51
                               }
                               $4 = HEAP32[$0 + 8 >> 2];
                               HEAP32[$4 >> 2] = $3 | 1;
                               HEAP32[$3 + 4 >> 2] = $4;
                               $2 = HEAP32[$3 >> 2];
                               HEAP32[$0 >> 2] = $2;
                               HEAP32[$0 + 8 >> 2] = $3;
                               HEAP32[$3 >> 2] = $0;
                               $2 = $2 & -4;
                               if (!$2) {
                                break label$49
                               }
                               if (HEAP32[$2 + 8 >> 2] == ($3 | 0)) {
                                break label$48
                               }
                               $2 = $2 + 4 | 0;
                               break label$47;
                              }
                              $0 = HEAP32[$3 + 8 >> 2];
                              if (HEAP8[$0 | 0] & 1) {
                               break label$50
                              }
                              $2 = HEAP32[$0 + 4 >> 2];
                              HEAP32[$2 >> 2] = $3 | 1;
                              HEAP32[$3 + 8 >> 2] = $2;
                              $5 = HEAP32[$3 >> 2];
                              HEAP32[$0 >> 2] = $5;
                              HEAP32[$0 + 4 >> 2] = $3;
                              HEAP32[$3 >> 2] = $0;
                              $5 = $5 & -4;
                              if (!$5) {
                               break label$44
                              }
                              if (HEAP32[$5 + 8 >> 2] == ($3 | 0)) {
                               break label$43
                              }
                              $5 = $5 + 4 | 0;
                              break label$42;
                             }
                             $4 = $0;
                             $5 = HEAP32[$0 + 4 >> 2];
                             if ($5) {
                              break label$46
                             }
                             break label$45;
                            }
                            $2 = $0;
                            $5 = HEAP32[$0 + 8 >> 2];
                            if ($5) {
                             break label$41
                            }
                            break label$40;
                           }
                           $2 = $1;
                           break label$47;
                          }
                          $2 = $2 + 8 | 0;
                         }
                         HEAP32[$2 >> 2] = $0;
                         $5 = HEAP32[$4 + 4 >> 2];
                         if (!$5) {
                          break label$45
                         }
                        }
                        if (!(HEAP8[$5 | 0] & 1)) {
                         break label$36
                        }
                       }
                       $2 = HEAP32[$4 + 8 >> 2];
                       if (HEAP8[$2 | 0] & 1 ? 0 : $2) {
                        break label$35
                       }
                       HEAP32[$4 >> 2] = $3;
                       $0 = HEAP32[$3 >> 2];
                       if (!($0 & 1)) {
                        break label$37
                       }
                       $2 = $0 & -4;
                       if ($2) {
                        break label$39
                       }
                       break label$3;
                      }
                      $5 = $1;
                      break label$42;
                     }
                     $5 = $5 + 8 | 0;
                    }
                    HEAP32[$5 >> 2] = $0;
                    $5 = HEAP32[$2 + 8 >> 2];
                    if (!$5) {
                     break label$40
                    }
                   }
                   if (!(HEAP8[$5 | 0] & 1)) {
                    break label$31
                   }
                  }
                  $4 = HEAP32[$2 + 4 >> 2];
                  if (HEAP8[$4 | 0] & 1 ? 0 : $4) {
                   break label$30
                  }
                  HEAP32[$2 >> 2] = $3;
                  $0 = HEAP32[$3 >> 2];
                  if (!($0 & 1)) {
                   break folding_inner0
                  }
                  $2 = $0 & -4;
                  if (!$2) {
                   break label$3
                  }
                 }
                 $4 = $3;
                 $3 = $2;
                 continue;
                }
                break;
               };
               break folding_inner0;
              }
              $2 = $4;
              $4 = $5;
              break label$34;
             }
             $0 = HEAP32[$2 + 4 >> 2];
             HEAP32[$4 + 8 >> 2] = $0;
             HEAP32[$2 + 4 >> 2] = $4;
             HEAP32[$3 + 4 >> 2] = $2;
             if (!$0) {
              break label$34
             }
             HEAP32[$0 >> 2] = $4 | 1;
            }
            $0 = HEAP32[$2 + 8 >> 2];
            HEAP32[$3 + 4 >> 2] = $0;
            $5 = $2 | 1;
            HEAP32[$4 >> 2] = $5;
            HEAP32[$2 + 8 >> 2] = $3;
            if ($0) {
             HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & 1 | $3
            }
            $0 = HEAP32[$3 >> 2];
            HEAP32[$2 >> 2] = $0;
            HEAP32[$3 >> 2] = $5;
            $0 = $0 & -4;
            if (!$0) {
             break label$32
            }
            if (HEAP32[$0 + 8 >> 2] == ($3 | 0)) {
             break label$28
            }
            HEAP32[$0 + 4 >> 2] = $2;
            return;
           }
           HEAP32[$1 >> 2] = $2;
           return;
          }
          $4 = $2;
          $2 = $5;
          break label$29;
         }
         $0 = HEAP32[$4 + 8 >> 2];
         HEAP32[$2 + 4 >> 2] = $0;
         HEAP32[$4 + 8 >> 2] = $2;
         HEAP32[$3 + 8 >> 2] = $4;
         if (!$0) {
          break label$29
         }
         HEAP32[$0 >> 2] = $2 | 1;
        }
        $0 = HEAP32[$4 + 4 >> 2];
        HEAP32[$3 + 8 >> 2] = $0;
        $5 = $2;
        $2 = $4 | 1;
        HEAP32[$5 >> 2] = $2;
        HEAP32[$4 + 4 >> 2] = $3;
        if ($0) {
         HEAP32[$0 >> 2] = HEAP32[$0 >> 2] & 1 | $3
        }
        $0 = HEAP32[$3 >> 2];
        HEAP32[$4 >> 2] = $0;
        HEAP32[$3 >> 2] = $2;
        $0 = $0 & -4;
        if (!$0) {
         break label$27
        }
        if (HEAP32[$0 + 8 >> 2] == ($3 | 0)) {
         break label$2
        }
        HEAP32[$0 + 4 >> 2] = $4;
        return;
       }
       HEAP32[$0 + 8 >> 2] = $2;
       return;
      }
      HEAP32[$1 >> 2] = $4;
     }
     return;
    }
    HEAP32[$0 + 8 >> 2] = $4;
    return;
   }
   HEAP32[$6 >> 2] = $5;
   HEAP32[$4 >> 2] = $3 | 1;
   return;
  }
  HEAP32[$3 >> 2] = $0 | 1;
 }
 
 function rb_next($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  label$1 : {
   $3 = HEAP32[$0 >> 2];
   if (($3 | 0) == ($0 | 0)) {
    break label$1
   }
   $1 = HEAP32[$0 + 4 >> 2];
   if ($1) {
    while (1) {
     $2 = $1;
     $1 = HEAP32[$1 + 8 >> 2];
     if ($1) {
      continue
     }
     break label$1;
    }
   }
   label$4 : {
    $1 = $3 & -4;
    if ($1) {
     while (1) {
      if (HEAP32[$1 + 4 >> 2] != ($0 | 0)) {
       break label$4
      }
      $0 = $1;
      $2 = HEAP32[$1 >> 2] & -4;
      $1 = $2;
      if ($1) {
       continue
      }
      break label$1;
     }
    }
    return $1;
   }
   return $1;
  }
  return $2;
 }
 
 function rb_prev($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  label$1 : {
   $3 = HEAP32[$0 >> 2];
   if (($3 | 0) == ($0 | 0)) {
    break label$1
   }
   $1 = HEAP32[$0 + 8 >> 2];
   if ($1) {
    while (1) {
     $2 = $1;
     $1 = HEAP32[$1 + 4 >> 2];
     if ($1) {
      continue
     }
     break label$1;
    }
   }
   label$4 : {
    $1 = $3 & -4;
    if ($1) {
     while (1) {
      if (HEAP32[$1 + 8 >> 2] != ($0 | 0)) {
       break label$4
      }
      $0 = $1;
      $2 = HEAP32[$1 >> 2] & -4;
      $1 = $2;
      if ($1) {
       continue
      }
      break label$1;
     }
    }
    return $1;
   }
   return $1;
  }
  return $2;
 }
 
 function radix_tree_node_rcu_free($0) {
  $0 = $0 | 0;
  memset($0 + 8 | 0, 0, 64);
  memset($0 + 72 | 0, 0, 12);
  HEAP32[$0 + 4 >> 2] = $0;
  HEAP32[$0 >> 2] = $0;
  kmem_cache_free(HEAP32[34974], $0 + -12 | 0);
 }
 
 function radix_tree_lookup($0) {
  var $1 = 0, $2 = 0, $3 = 0;
  label$1 : {
   label$2 : while (1) {
    $2 = 0;
    $1 = HEAP32[973];
    if (($1 & 3) == 2) {
     $3 = (16 << HEAPU8[$1 & -3]) + -1 | 0
    } else {
     $3 = 0
    }
    if ($3 >>> 0 < $0 >>> 0) {
     break label$1
    }
    while (1) {
     if (($1 & 3) == 2) {
      if (($1 | 0) == 1030) {
       continue label$2
      }
      $1 = $1 & -3;
      $2 = HEAPU8[$1 | 0];
      $1 = HEAP32[((($0 >>> $2 & 15) << 2) + $1 | 0) + 20 >> 2];
      if ($2) {
       continue
      }
     }
     break;
    };
    break;
   };
   $2 = $1;
  }
  return $2;
 }
 
 function timerqueue_add($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  $2 = HEAP32[$0 >> 2];
  label$1 : {
   if ($2) {
    $6 = HEAP32[$1 + 16 >> 2];
    $5 = HEAP32[$1 + 20 >> 2];
    while (1) {
     $4 = $2;
     $7 = $2;
     $2 = $2 + 16 | 0;
     $3 = HEAP32[$2 >> 2];
     $2 = HEAP32[$2 + 4 >> 2];
     if (($5 | 0) < ($2 | 0)) {
      $3 = 1
     } else {
      $3 = ($5 | 0) <= ($2 | 0) ? ($6 >>> 0 >= $3 >>> 0 ? 0 : 1) : 0
     }
     $2 = HEAP32[$7 + ($3 ? 8 : 4) >> 2];
     if ($2) {
      continue
     }
     break;
    };
    $2 = $3 ? $4 + 8 | 0 : $4 + 4 | 0;
    break label$1;
   }
   $2 = $0;
  }
  HEAP32[$1 + 4 >> 2] = 0;
  HEAP32[$1 + 8 >> 2] = 0;
  HEAP32[$1 >> 2] = $4;
  HEAP32[$2 >> 2] = $1;
  rb_insert_color($1, $0);
  $2 = HEAP32[$0 + 4 >> 2];
  label$4 : {
   if ($2) {
    $4 = 0;
    $6 = HEAP32[$1 + 16 >> 2];
    $5 = HEAP32[$2 + 16 >> 2];
    $3 = HEAP32[$1 + 20 >> 2];
    $2 = HEAP32[$2 + 20 >> 2];
    if (($3 | 0) > ($2 | 0)) {
     $2 = 1
    } else {
     $2 = ($3 | 0) >= ($2 | 0) ? ($6 >>> 0 < $5 >>> 0 ? 0 : 1) : 0
    }
    if ($2) {
     break label$4
    }
   }
   HEAP32[$0 + 4 >> 2] = $1;
   $4 = 1;
  }
  return $4;
 }
 
 function timerqueue_del($0, $1) {
  var wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  if (($1 | 0) == HEAP32[$0 + 4 >> 2]) {
   (wasm2js_i32$0 = $0 + 4 | 0, wasm2js_i32$1 = rb_next($1)), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1
  }
  rb_erase($1, $0);
  HEAP32[$1 >> 2] = $1;
  return HEAP32[$0 + 4 >> 2] != 0;
 }
 
 function xas_load($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0;
  folding_inner0 : {
   label$1 : {
    label$2 : {
     label$3 : {
      label$4 : {
       label$5 : {
        label$6 : {
         $2 = HEAP32[$0 + 12 >> 2];
         $3 = $2 & 3;
         if ($3) {
          if (($3 | 0) == 2 ? $2 >>> 0 >= 4 : 0) {
           break label$2
          }
          $2 = HEAP32[$0 + 4 >> 2];
          $1 = HEAP32[HEAP32[$0 >> 2] + 4 >> 2];
          if ($1 >>> 0 < 4097) {
           break label$6
          }
          if (($1 & 3) != 2) {
           break label$6
          }
          if ($2 >>> HEAPU8[$1 + -2 | 0] >>> 0 < 16) {
           break label$4
          }
          break folding_inner0;
         }
         if (!$2) {
          break label$5
         }
         $1 = HEAP32[($2 + (HEAPU8[$0 + 10 | 0] << 2) | 0) + 20 >> 2];
         break label$3;
        }
        if (!$2) {
         break label$4
        }
        break folding_inner0;
       }
       $1 = HEAP32[HEAP32[$0 >> 2] + 4 >> 2];
       break label$3;
      }
      HEAP32[$0 + 12 >> 2] = 0;
     }
     if ($1 >>> 0 < 4097) {
      break label$2
     }
     if (($1 & 3) != 2) {
      break label$2
     }
     $6 = HEAPU8[$0 + 8 | 0];
     $2 = $1 + -2 | 0;
     $4 = HEAPU8[$2 | 0];
     if ($6 >>> 0 > $4 >>> 0) {
      break label$2
     }
     $7 = HEAP32[$0 + 4 >> 2];
     while (1) {
      $3 = $2;
      $5 = $7 >>> $4 & 15;
      $1 = HEAP32[($2 + ($5 << 2) | 0) + 20 >> 2];
      if ($1 >>> 0 < 4097) {
       break label$1
      }
      if (($1 & 3) != 2) {
       break label$1
      }
      $2 = $1 + -2 | 0;
      $4 = HEAPU8[$2 | 0];
      if ($6 >>> 0 <= $4 >>> 0) {
       continue
      }
      break;
     };
     HEAP8[$0 + 10 | 0] = $5;
     HEAP32[$0 + 12 >> 2] = $3;
    }
    return $1;
   }
   HEAP8[$0 + 10 | 0] = $5;
   HEAP32[$0 + 12 >> 2] = $3;
   return $1;
  }
  HEAP32[$0 + 12 >> 2] = 1;
  return 0;
 }
 
 function xas_create($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0;
  $16 = HEAPU8[$0 + 8 | 0];
  $8 = HEAP32[$0 >> 2];
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       label$6 : {
        label$7 : {
         label$8 : {
          $1 = HEAP32[$0 + 12 >> 2];
          if ($1 >>> 0 > 3) {
           if ($1 >>> 0 >= 4) {
            if (($1 & 3) == 2) {
             break label$2
            }
           }
           if (!$1) {
            break label$8
           }
           $7 = ((HEAPU8[$0 + 10 | 0] << 2) + $1 | 0) + 20 | 0;
           $6 = HEAPU8[$1 | 0];
           break label$3;
          }
          HEAP32[$0 + 12 >> 2] = 0;
          $7 = $8 + 4 | 0;
          $11 = HEAP32[$0 + 4 >> 2];
          $5 = HEAP32[$8 + 4 >> 2];
          if ($5) {
           if ($5 >>> 0 < 4097) {
            break label$6
           }
           $1 = 0;
           if (($5 & 3) != 2) {
            break label$5
           }
           $1 = $5 + -2 | 0;
           $6 = HEAPU8[$1 | 0] + 4 | 0;
           break label$5;
          }
          if (!$11) {
           break label$7
          }
          $1 = 0;
          while (1) {
           $2 = $11 >>> $3 | 0;
           $6 = $3 + 4 | 0;
           $3 = $6;
           if ($2 >>> 0 > 15) {
            continue
           }
           break;
          };
          break label$4;
         }
         $7 = $8 + 4 | 0;
        }
        return HEAP32[$7 >> 2];
       }
       $1 = 0;
      }
      $10 = $0 + 12 | 0;
      HEAP32[$10 >> 2] = 0;
      $12 = $0 + 20 | 0;
      $15 = $0 + 10 | 0;
      label$13 : while (1) {
       $3 = 0;
       label$17 : {
        label$18 : {
         $2 = $11;
         $13 = ($5 & 3) == 2 & $5 >>> 0 > 4096;
         if (($13 | 0) == 1) {
          $4 = (16 << HEAPU8[$5 + -2 | 0]) + -1 | 0
         } else {
          $4 = 0
         }
         if ($2 >>> 0 > $4 >>> 0) {
          $4 = HEAP32[$10 >> 2];
          if ($4 & 3) {
           break label$2
          }
          $2 = $0 + 16 | 0;
          $1 = HEAP32[$2 >> 2];
          label$20 : {
           label$21 : {
            if ($1) {
             HEAP32[$2 >> 2] = 0;
             if ($4) {
              break label$21
             }
             break label$20;
            }
            $1 = kmem_cache_alloc(HEAP32[34974], 4194816);
            if (!$1) {
             break label$18
            }
            if (!$4) {
             break label$20
            }
           }
           HEAP8[$1 + 1 | 0] = HEAPU8[$15 | 0];
           HEAP8[$4 + 2 | 0] = HEAPU8[$4 + 2 | 0] + 1;
           $2 = HEAP32[$12 >> 2];
           if (!$2) {
            break label$20
           }
           FUNCTION_TABLE[$2]($4);
          }
          HEAP16[$1 + 2 >> 1] = 0;
          HEAP8[$1 | 0] = $6;
          HEAP32[$1 + 8 >> 2] = HEAP32[$0 >> 2];
          HEAP32[$1 + 4 >> 2] = HEAP32[$10 >> 2];
          if (!$1) {
           break label$2
          }
          HEAP8[$1 + 2 | 0] = 1;
          if ($5 & 1) {
           HEAP8[$1 + 3 | 0] = 1
          }
          HEAP32[$1 + 20 >> 2] = $5;
          $9 = HEAP32[$8 >> 2];
          $4 = $1 + 84 | 0;
          $2 = $4;
          break label$17;
         }
         HEAP32[$0 + 12 >> 2] = $1;
         break label$4;
        }
        HEAP32[$0 + 12 >> 2] = -46;
        return 0;
       }
       $14 = 0;
       while (1) {
        label$25 : {
         label$26 : {
          label$27 : {
           label$28 : {
            if (!$14) {
             if (!($9 & 4)) {
              break label$25
             }
             memset($4, 255, 4);
             $9 = HEAP32[$8 >> 2];
             if ($9 & 8388608) {
              break label$28
             }
             $9 = $9 | 8388608;
             HEAP32[$8 >> 2] = $9;
             HEAP32[$4 >> 2] = HEAP32[$4 >> 2] & -2;
             break label$28;
            }
            if (8388608 << $3 & $9) {
             HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 1
            }
            if (($3 | 0) == 2) {
             break label$27
            }
           }
           $2 = $2 + 4 | 0;
           $3 = $3 + 1 | 0;
           if ($3) {
            break label$26
           }
           $14 = 0;
           continue;
          }
          if ($13) {
           HEAP8[$5 + -1 | 0] = 0;
           HEAP32[$5 + 2 >> 2] = $1;
          }
          $5 = $1 | 2;
          HEAP32[$7 >> 2] = $5;
          $2 = HEAP32[$12 >> 2];
          if ($2) {
           FUNCTION_TABLE[$2]($1)
          }
          $6 = $6 + 4 | 0;
          continue label$13;
         }
         $14 = 1;
         continue;
        }
        $14 = 1;
        continue;
       };
      };
     }
     $3 = 0;
     if (($6 | 0) < 0) {
      break label$2
     }
    }
    $3 = HEAP32[$7 >> 2];
    if ($6 >>> 0 <= $16 >>> 0) {
     break label$2
    }
    $4 = $0 + 16 | 0;
    $10 = $0 + 12 | 0;
    $12 = $0 + 10 | 0;
    $15 = $0 + 20 | 0;
    while (1) {
     $6 = $6 + -4 | 0;
     $11 = $10;
     label$35 : {
      if ($3) {
       if ($3 >>> 0 < 4097) {
        break label$2
       }
       if (($3 & 3) != 2) {
        break label$2
       }
       $2 = $3 + -2 | 0;
       break label$35;
      }
      $3 = 0;
      if ($1 & 3) {
       break label$2
      }
      $2 = HEAP32[$4 >> 2];
      label$37 : {
       label$38 : {
        if ($2) {
         HEAP32[$4 >> 2] = 0;
         if ($1) {
          break label$38
         }
         break label$37;
        }
        $2 = kmem_cache_alloc(HEAP32[34974], 4194816);
        if (!$2) {
         break label$1
        }
        if (!$1) {
         break label$37
        }
       }
       HEAP8[$2 + 1 | 0] = HEAPU8[$12 | 0];
       HEAP8[$1 + 2 | 0] = HEAPU8[$1 + 2 | 0] + 1;
       $13 = HEAP32[$15 >> 2];
       if (!$13) {
        break label$37
       }
       FUNCTION_TABLE[$13]($1);
      }
      HEAP16[$2 + 2 >> 1] = 0;
      HEAP8[$2 | 0] = $6;
      HEAP32[$2 + 8 >> 2] = HEAP32[$0 >> 2];
      HEAP32[$2 + 4 >> 2] = HEAP32[$10 >> 2];
      if (!$2) {
       break label$2
      }
      if (HEAPU8[$8 | 0] & 4) {
       memset($2 + 84 | 0, 255, 4)
      }
      HEAP32[$7 >> 2] = $2 | 2;
     }
     $1 = $2;
     HEAP32[$11 >> 2] = $1;
     $2 = HEAP32[$0 + 4 >> 2] >>> HEAPU8[$1 | 0] & 15;
     HEAP8[$12 | 0] = $2;
     $7 = (($2 << 2) + $1 | 0) + 20 | 0;
     $3 = HEAP32[$7 >> 2];
     if ($6 >>> 0 > $16 >>> 0) {
      continue
     }
     break;
    };
   }
   return $3;
  }
  HEAP32[$0 + 12 >> 2] = -46;
  return 0;
 }
 
 function xas_store($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0;
  $7 = HEAP32[$0 >> 2];
  label$1 : {
   if ($1) {
    $9 = xas_create($0);
    break label$1;
   }
   $9 = xas_load($0);
  }
  label$3 : {
   $5 = HEAP32[$0 + 12 >> 2];
   if ($5 & 3) {
    break label$3
   }
   label$4 : {
    if (!$5) {
     break label$4
    }
    if (HEAPU8[$0 + 8 | 0] >= HEAPU8[$5 | 0]) {
     break label$4
    }
    HEAP8[$0 + 9 | 0] = 0;
   }
   $2 = HEAPU8[$0 + 9 | 0];
   label$5 : {
    if (($1 | 0) != ($9 | 0)) {
     break label$5
    }
    if ($2) {
     break label$5
    }
    return;
   }
   $8 = HEAPU8[$0 + 10 | 0];
   $14 = $2 + $8 | 0;
   label$6 : {
    label$7 : {
     if ($5) {
      $12 = (($8 << 2) + $5 | 0) + 20 | 0;
      label$9 : {
       if (!$2) {
        break label$9
       }
       $10 = $14 + 1 | 0;
       $3 = 84;
       $15 = $0 + 12 | 0;
       $13 = $0 + 10 | 0;
       $17 = $0 + 9 | 0;
       $2 = $5;
       $7 = $8;
       while (1) {
        $16 = $2 + $3 | 0;
        if ((find_next_bit($16, $10, ($7 & 255) + 1 | 0) | 0) != ($10 | 0)) {
         $7 = HEAPU8[$13 | 0];
         $2 = ($2 + ($7 >>> 3 & 28) | 0) + $3 | 0;
         HEAP32[$2 >> 2] = HEAP32[$2 >> 2] | 1 << ($7 & 31);
         __bitmap_clear($16, $7 + 1 | 0, HEAPU8[$17 | 0]);
        }
        if (($3 | 0) == 92) {
         break label$9
        }
        $3 = $3 + 4 | 0;
        $2 = HEAP32[$15 >> 2];
        $7 = HEAPU8[$13 | 0];
        continue;
       };
      }
      $3 = $1 & 1;
      if (!$1) {
       break label$7
      }
      break label$6;
     }
     $12 = $7 + 4 | 0;
     $3 = $1 & 1;
     if ($1) {
      break label$6
     }
    }
    xas_init_marks($0);
   }
   $13 = $3 ^ 1;
   $15 = $0 + 10 | 0;
   $10 = 0;
   $7 = 0;
   $3 = 0;
   while (1) {
    label$13 : {
     label$14 : {
      label$15 : {
       if (!$3) {
        HEAP32[$12 >> 2] = $1;
        if ($9 >>> 0 < 4097) {
         break label$15
        }
        if (($9 & 3) != 2) {
         break label$15
        }
        $4 = 0;
        $11 = $9 + -2 | 0;
        $6 = $11;
        break label$14;
       }
       label$18 : {
        $3 = (($4 << 2) + $6 | 0) + 20 | 0;
        $2 = HEAP32[$3 >> 2];
        if ($2 >>> 0 < 4097) {
         break label$18
        }
        if (($2 & 3) != 2) {
         break label$18
        }
        $6 = $2 + -2 | 0;
        $4 = 0;
        $3 = 1;
        continue;
       }
       if ($2) {
        HEAP32[$3 >> 2] = 1030
       }
       while (1) {
        $4 = $4 + 1 | 0;
        if (($4 | 0) != 16) {
         break label$13
        }
        HEAP16[$6 + 2 >> 1] = 0;
        $4 = HEAPU8[$6 + 1 | 0];
        $2 = HEAP32[$6 + 4 >> 2];
        $3 = HEAP32[$0 + 20 >> 2];
        if ($3) {
         FUNCTION_TABLE[$3]($6)
        }
        HEAP32[$6 + 8 >> 2] = 1;
        call_rcu($6 + 12 | 0, 106);
        $3 = ($6 | 0) != ($11 | 0);
        $6 = $2;
        if ($3) {
         continue
        }
        break;
       };
      }
      if (!$5) {
       break label$3
      }
      $10 = (!$9 - !$1 | 0) + $10 | 0;
      $7 = ((($9 ^ -1) & 1) - $13 | 0) + $7 | 0;
      label$22 : {
       label$23 : {
        if ($1) {
         if (($8 | 0) == ($14 | 0)) {
          break label$22
         }
         $8 = $8 + 1 | 0;
         $1 = HEAPU8[$15 | 0] << 2 | 2;
         break label$23;
        }
        if (($8 | 0) == 15) {
         break label$22
        }
        $1 = 0;
        $8 = $8 + 1 | 0;
        if ($8 >>> 0 > $14 >>> 0) {
         break label$22
        }
       }
       $12 = $12 + 4 | 0;
       $9 = HEAP32[(($8 << 2) + $5 | 0) + 20 >> 2];
       $3 = 0;
       continue;
      }
      if (!($7 | $10)) {
       break label$3
      }
      HEAP8[$5 + 2 | 0] = HEAPU8[$5 + 2 | 0] + $10;
      HEAP8[$5 + 3 | 0] = HEAPU8[$5 + 3 | 0] + $7;
      $1 = HEAP32[$0 + 20 >> 2];
      if ($1) {
       FUNCTION_TABLE[$1]($5)
      }
      if (($10 | 0) > -1) {
       break label$3
      }
      $6 = $0 + 12 | 0;
      $4 = HEAP32[$6 >> 2];
      $2 = HEAP32[$4 + 4 >> 2];
      label$26 : {
       if (!HEAPU8[$4 + 2 | 0]) {
        $5 = $0 + 10 | 0;
        while (1) {
         HEAP32[$4 + 8 >> 2] = 1;
         $1 = $2;
         HEAP32[$6 >> 2] = $1;
         HEAP8[$5 | 0] = HEAPU8[$4 + 1 | 0];
         call_rcu($4 + 12 | 0, 106);
         if (!$1) {
          break label$26
         }
         HEAP32[($1 + (HEAPU8[$5 | 0] << 2) | 0) + 20 >> 2] = 0;
         $3 = HEAPU8[$1 + 2 | 0] + -1 | 0;
         HEAP8[$1 + 2 | 0] = $3;
         $2 = HEAP32[$0 + 20 >> 2];
         if ($2) {
          FUNCTION_TABLE[$2]($1);
          $3 = HEAPU8[$1 + 2 | 0];
         }
         $2 = HEAP32[$1 + 4 >> 2];
         $4 = $1;
         if (!($3 & 255)) {
          continue
         }
         break;
        };
       }
       if ($2) {
        break label$3
       }
       $6 = $0 + 12 | 0;
       $4 = HEAP32[$6 >> 2];
       if (HEAPU8[$4 + 2 | 0] != 1) {
        break label$3
       }
       $11 = $4 + 2 | 0;
       $1 = HEAP32[$0 >> 2];
       $8 = $0 + 20 | 0;
       while (1) {
        $0 = HEAP32[$4 + 20 >> 2];
        if (!$0) {
         break label$3
        }
        $2 = ($0 & 3) == 2 & $0 >>> 0 > 4096;
        if (HEAPU8[$4 | 0] ? !$2 : 0) {
         break label$3
        }
        HEAP32[$1 + 4 >> 2] = $0;
        HEAP32[$6 >> 2] = 1;
        $5 = HEAP32[$1 >> 2];
        label$32 : {
         if (!($5 & 4)) {
          break label$32
         }
         if (!($5 & 8388608)) {
          break label$32
         }
         if (HEAP32[$4 + 84 >> 2] & 1) {
          break label$32
         }
         HEAP32[$1 >> 2] = $5 & -8388609;
        }
        HEAP8[$4 + 3 | 0] = 0;
        HEAP8[$11 | 0] = 0;
        if (!$2) {
         HEAP32[$4 + 20 >> 2] = 1030
        }
        $5 = HEAP32[$8 >> 2];
        if ($5) {
         FUNCTION_TABLE[$5]($4)
        }
        HEAP32[$4 + 8 >> 2] = 1;
        call_rcu($4 + 12 | 0, 106);
        if (!$2) {
         break label$3
        }
        HEAP32[$0 + 2 >> 2] = 0;
        $4 = $0 + -2 | 0;
        $11 = $0;
        if (HEAPU8[$0 | 0] == 1) {
         continue
        }
        break;
       };
       break label$3;
      }
      HEAP32[HEAP32[$0 >> 2] + 4 >> 2] = 0;
      HEAP32[$0 + 12 >> 2] = 1;
      return;
     }
     $3 = 1;
     continue;
    }
    $3 = 1;
    continue;
   };
  }
 }
 
 function xas_init_marks($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, wasm2js_i32$0 = 0, wasm2js_i32$1 = 0;
  $7 = $0 + 10 | 0;
  $4 = HEAP32[$0 + 12 >> 2];
  $8 = $4 & 3;
  $9 = HEAP32[$0 >> 2];
  $3 = HEAP32[$9 >> 2];
  $0 = 0;
  while (1) {
   label$4 : {
    label$5 : {
     label$6 : {
      $11 = $9;
      label$7 : {
       label$8 : {
        label$9 : {
         if (!$0) {
          if (!($3 & 4)) {
           break label$4
          }
          if ($8) {
           break label$7
          }
          if ($4) {
           $1 = $7;
           $0 = $4;
           while (1) {
            $1 = HEAPU8[$1 | 0];
            $2 = (($1 >>> 3 & 28) + $0 | 0) + 84 | 0;
            $6 = $2;
            $1 = 1 << ($1 & 31);
            $2 = HEAP32[$2 >> 2];
            HEAP32[$6 >> 2] = $1 | $2;
            if ($1 & $2) {
             break label$8
            }
            $1 = $0 + 1 | 0;
            $0 = HEAP32[$0 + 4 >> 2];
            if ($0) {
             continue
            }
            break;
           };
          }
          if ($3 & 8388608) {
           break label$8
          }
          $0 = $3 | 8388608;
          break label$9;
         }
         if ($8) {
          break label$8
         }
         if ($4) {
          $1 = $7;
          $0 = $4;
          while (1) {
           $10 = ($5 << 2) + $0 | 0;
           $1 = HEAPU8[$1 | 0];
           $6 = ($10 + ($1 >>> 3 & 28) | 0) + 84 | 0;
           $2 = HEAP32[$6 >> 2];
           (wasm2js_i32$0 = $6, wasm2js_i32$1 = __wasm_rotl_i32(-2, $1) & $2), HEAP32[wasm2js_i32$0 >> 2] = wasm2js_i32$1;
           if (!($2 & 1 << ($1 & 31))) {
            break label$8
           }
           if (HEAPU16[$10 + 84 >> 1]) {
            break label$8
           }
           $1 = $0 + 1 | 0;
           $0 = HEAP32[$0 + 4 >> 2];
           if ($0) {
            continue
           }
           break;
          };
         }
         $0 = 8388608 << $5;
         if (!($0 & $3)) {
          break label$8
         }
         $0 = ($0 ^ -1) & $3;
        }
        $3 = $0;
        HEAP32[$11 >> 2] = $3;
       }
       if (($5 | 0) == 2) {
        break label$6
       }
      }
      $5 = $5 + 1 | 0;
      if ($5) {
       break label$5
      }
      $0 = 0;
      continue;
     }
     return;
    }
    $0 = 1;
    continue;
   }
   $0 = 1;
   continue;
  };
 }
 
 function idr_find($0) {
  return radix_tree_lookup($0 - HEAP32[974] | 0);
 }
 
 function int_sqrt($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  if ($0 >>> 0 < 2) {
   return $0
  }
  $1 = $0 >>> 0 > 65535;
  $2 = $1 ? 31 : 15;
  $3 = $2;
  $4 = $2 + -8 | 0;
  $1 = $1 ? $0 : $0 << 16;
  $2 = $1 >>> 0 > 16777215;
  $3 = $2 ? $3 : $4;
  $1 = $2 ? $1 : $1 << 8;
  $2 = $1 >>> 0 > 268435455;
  $3 = $2 ? $3 : $3 + -4 | 0;
  $1 = $2 ? $1 : $1 << 4;
  $2 = $1 >>> 0 > 1073741823;
  $1 = 1 << (($2 ? $3 : $3 + -2 | 0) + (($2 ? $1 : $1 << 2) >> 31 ^ -1) & -2);
  $2 = 0;
  while (1) {
   $3 = $1 + $2 | 0;
   $4 = $0 >>> 0 < $3 >>> 0;
   $2 = ($2 >>> 1 | 0) + ($4 ? 0 : $1) | 0;
   $0 = $0 - ($4 ? 0 : $3) | 0;
   $1 = $1 >>> 2 | 0;
   if ($1) {
    continue
   }
   break;
  };
  return $2;
 }
 
 function cmp_ex_sort($0, $1) {
  $0 = $0 | 0;
  $1 = $1 | 0;
  $0 = HEAP32[$0 >> 2];
  $1 = HEAP32[$1 >> 2];
  return ($0 >>> 0 > $1 >>> 0 ? 1 : $0 >>> 0 < $1 >>> 0 ? -1 : 0) | 0;
 }
 
 function ___ratelimit($0, $1) {
  var $2 = 0, $3 = 0;
  $3 = global$0 - 16 | 0;
  global$0 = $3;
  label$1 : {
   label$2 : {
    label$3 : {
     label$4 : {
      label$5 : {
       if (HEAP32[$0 >> 2]) {
        $2 = HEAP32[$0 + 16 >> 2];
        if (!$2) {
         $2 = HEAP32[20745];
         HEAP32[$0 + 16 >> 2] = $2;
        }
        if (((HEAP32[$0 >> 2] + $2 | 0) - HEAP32[20745] | 0) <= -1) {
         break label$5
        }
        $1 = HEAP32[$0 + 4 >> 2];
        if ($1) {
         break label$4
        }
        break label$3;
       }
       $0 = 1;
       break label$1;
      }
      $2 = HEAP32[$0 + 12 >> 2];
      label$8 : {
       if (!$2) {
        break label$8
       }
       if (HEAP8[$0 + 20 | 0] & 1) {
        break label$8
       }
       HEAP32[$3 + 4 >> 2] = $2;
       HEAP32[$3 >> 2] = $1;
       printk_deferred(32601, $3);
       HEAP32[$0 + 12 >> 2] = 0;
      }
      HEAP32[$0 + 8 >> 2] = 0;
      HEAP32[$0 + 16 >> 2] = HEAP32[20745];
      $1 = HEAP32[$0 + 4 >> 2];
      if (!$1) {
       break label$3
      }
     }
     $2 = HEAP32[$0 + 8 >> 2];
     if (($1 | 0) <= ($2 | 0)) {
      break label$3
     }
     $1 = $0 + 8 | 0;
     $0 = 1;
     break label$2;
    }
    $1 = $0 + 12 | 0;
    $2 = HEAP32[$0 + 12 >> 2];
    $0 = 0;
   }
   HEAP32[$1 >> 2] = $2 + 1;
  }
  global$0 = $3 + 16 | 0;
  return $0;
 }
 
 function show_mem($0, $1) {
  var $2 = 0, $3 = 0, $4 = 0;
  $2 = global$0 - 48 | 0;
  global$0 = $2;
  printk(32632, 0);
  show_free_areas($0, $1);
  $1 = 0;
  $0 = 141272;
  if ($0) {
   while (1) {
    $3 = HEAP32[$0 + 48 >> 2];
    if ($3) {
     $4 = ($3 + $4 | 0) - HEAP32[$0 + 40 >> 2] | 0;
     $1 = $1 + $3 | 0;
    }
    $3 = HEAP32[$0 + 564 >> 2];
    if ($3) {
     $4 = ($3 + $4 | 0) - HEAP32[$0 + 556 >> 2] | 0;
     $1 = $1 + $3 | 0;
    }
    $0 = 0;
    if ($0) {
     continue
    }
    break;
   }
  }
  HEAP32[$2 + 32 >> 2] = $1;
  printk(32643, $2 + 32 | 0);
  HEAP32[$2 + 16 >> 2] = 0;
  printk(32658, $2 + 16 | 0);
  HEAP32[$2 >> 2] = $4;
  printk(32689, $2);
  global$0 = $2 + 48 | 0;
 }
 
 function siphash_1u32($0) {
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0;
  $9 = HEAP32[34973];
  $1 = $9 ^ 1685025377;
  $2 = $1;
  $6 = HEAP32[34972];
  $3 = $6 ^ 1852075885;
  $7 = __wasm_rotl_i64($3, $1, 13);
  $4 = i64toi32_i32$HIGH_BITS;
  $1 = HEAP32[34971];
  $8 = $1;
  $1 = $2 + ($1 ^ 1936682341) | 0;
  $5 = HEAP32[34970];
  $2 = $3 + ($5 ^ 1886610805) | 0;
  if ($2 >>> 0 < $3 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $7 = $2 ^ $7;
  $3 = $1;
  $10 = $1 ^ $4;
  $11 = __wasm_rotl_i64($7, $10, 17);
  $12 = $9 ^ 1885693026;
  $1 = $12 + ($8 ^ 1819895653) | 0;
  $9 = $0;
  $8 = $6 ^ $0 ^ 2037671283;
  $0 = $8;
  $4 = $0 + ($5 ^ 1852142177) | 0;
  if ($4 >>> 0 < $0 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $5 = i64toi32_i32$HIGH_BITS;
  $0 = $1 + $10 | 0;
  $6 = $4 + $7 | 0;
  if ($6 >>> 0 < $4 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $10 = $6 ^ $11;
  $7 = $0;
  $5 = $0 ^ $5;
  $11 = __wasm_rotl_i64($10, $5, 13);
  $13 = i64toi32_i32$HIGH_BITS;
  $3 = __wasm_rotl_i64($2, $3, 32);
  $0 = i64toi32_i32$HIGH_BITS;
  $2 = __wasm_rotl_i64($8, $12, 16) ^ $4;
  $8 = $1 ^ i64toi32_i32$HIGH_BITS;
  $0 = $8 + $0 | 0;
  $4 = $2 + $3 | 0;
  if ($4 >>> 0 < $2 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $1 = $0 + $5 | 0;
  $3 = $4 + $10 | 0;
  if ($3 >>> 0 < $4 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $5 = $3 ^ $11;
  $10 = $1 ^ $13;
  $11 = __wasm_rotl_i64($5, $10, 17);
  $12 = i64toi32_i32$HIGH_BITS;
  $4 = __wasm_rotl_i64($2, $8, 21) ^ $4;
  $8 = $0 ^ i64toi32_i32$HIGH_BITS;
  $0 = __wasm_rotl_i64($6, $7, 32);
  $7 = $0 + $4 | 0;
  $6 = $10;
  $2 = i64toi32_i32$HIGH_BITS + $8 | 0;
  $10 = $7 >>> 0 < $0 >>> 0 ? $2 + 1 | 0 : $2;
  $0 = $6 + $10 | 0;
  $2 = $7 + $5 | 0;
  if ($2 >>> 0 < $7 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $12 = $0 ^ $12;
  $13 = $12;
  $6 = $2;
  $5 = $2 ^ $11;
  $11 = __wasm_rotl_i64($5, $12, 13);
  $12 = i64toi32_i32$HIGH_BITS;
  $7 = __wasm_rotl_i64($4, $8, 16) ^ $7;
  $4 = $10 ^ i64toi32_i32$HIGH_BITS;
  $1 = __wasm_rotl_i64($3, $1, 32);
  $2 = $1 + $7 | 0;
  $3 = i64toi32_i32$HIGH_BITS + $4 | 0;
  $8 = $2 >>> 0 < $1 >>> 0 ? $3 + 1 | 0 : $3;
  $1 = $13 + ($8 ^ 67108864) | 0;
  $3 = $5 + ($2 ^ $9) | 0;
  if ($3 >>> 0 < $5 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $5 = $3 ^ $11;
  $9 = $1;
  $1 = $1 ^ $12;
  $10 = __wasm_rotl_i64($5, $1, 17);
  $11 = i64toi32_i32$HIGH_BITS;
  $6 = __wasm_rotl_i64($6, $0, 32) ^ 255;
  $0 = i64toi32_i32$HIGH_BITS;
  $2 = __wasm_rotl_i64($7, $4, 21) ^ $2;
  $4 = $8 ^ i64toi32_i32$HIGH_BITS;
  $0 = $4 + $0 | 0;
  $7 = $2 + $6 | 0;
  if ($7 >>> 0 < $2 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $1 = $0 + $1 | 0;
  $6 = $7 + $5 | 0;
  if ($6 >>> 0 < $7 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $8 = $6 ^ $10;
  $5 = $1 ^ $11;
  $10 = __wasm_rotl_i64($8, $5, 13);
  $11 = i64toi32_i32$HIGH_BITS;
  $7 = __wasm_rotl_i64($2, $4, 16) ^ $7;
  $4 = $0 ^ i64toi32_i32$HIGH_BITS;
  $0 = __wasm_rotl_i64($3, $9, 32);
  $3 = $0 + $7 | 0;
  $2 = i64toi32_i32$HIGH_BITS + $4 | 0;
  $9 = $3 >>> 0 < $0 >>> 0 ? $2 + 1 | 0 : $2;
  $0 = $9 + $5 | 0;
  $2 = $3 + $8 | 0;
  if ($2 >>> 0 < $3 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $8 = $2 ^ $10;
  $5 = $0 ^ $11;
  $10 = __wasm_rotl_i64($8, $5, 17);
  $11 = i64toi32_i32$HIGH_BITS;
  $7 = __wasm_rotl_i64($7, $4, 21) ^ $3;
  $4 = $9 ^ i64toi32_i32$HIGH_BITS;
  $1 = __wasm_rotl_i64($6, $1, 32);
  $9 = $1 + $7 | 0;
  $3 = i64toi32_i32$HIGH_BITS + $4 | 0;
  $6 = $9 >>> 0 < $1 >>> 0 ? $3 + 1 | 0 : $3;
  $1 = $6 + $5 | 0;
  $3 = $8 + $9 | 0;
  if ($3 >>> 0 < $9 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $8 = $3 ^ $10;
  $5 = $1 ^ $11;
  $10 = __wasm_rotl_i64($8, $5, 13);
  $11 = i64toi32_i32$HIGH_BITS;
  $7 = __wasm_rotl_i64($7, $4, 16) ^ $9;
  $4 = $6 ^ i64toi32_i32$HIGH_BITS;
  $0 = __wasm_rotl_i64($2, $0, 32);
  $6 = $0 + $7 | 0;
  $9 = $5;
  $2 = i64toi32_i32$HIGH_BITS + $4 | 0;
  $5 = $6 >>> 0 < $0 >>> 0 ? $2 + 1 | 0 : $2;
  $0 = $9 + $5 | 0;
  $2 = $6 + $8 | 0;
  if ($2 >>> 0 < $6 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $8 = $2 ^ $10;
  $9 = $0;
  $0 = $0 ^ $11;
  $10 = __wasm_rotl_i64($8, $0, 17);
  $11 = i64toi32_i32$HIGH_BITS;
  $7 = __wasm_rotl_i64($7, $4, 21) ^ $6;
  $4 = $5 ^ i64toi32_i32$HIGH_BITS;
  $1 = __wasm_rotl_i64($3, $1, 32);
  $6 = $1 + $7 | 0;
  $3 = $0;
  $0 = i64toi32_i32$HIGH_BITS + $4 | 0;
  $5 = $6 >>> 0 < $1 >>> 0 ? $0 + 1 | 0 : $0;
  $0 = $3 + $5 | 0;
  $1 = $6 + $8 | 0;
  if ($1 >>> 0 < $6 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $3 = $1;
  $8 = $1 ^ $10;
  $1 = $0 ^ $11;
  $10 = __wasm_rotl_i64($8, $1, 13);
  $11 = i64toi32_i32$HIGH_BITS;
  $6 = __wasm_rotl_i64($7, $4, 16) ^ $6;
  $7 = $5 ^ i64toi32_i32$HIGH_BITS;
  $9 = __wasm_rotl_i64($2, $9, 32);
  $2 = $9 + $6 | 0;
  $5 = $1;
  $1 = i64toi32_i32$HIGH_BITS + $7 | 0;
  $9 = $2 >>> 0 < $9 >>> 0 ? $1 + 1 | 0 : $1;
  $1 = $5 + $9 | 0;
  $4 = $2 + $8 | 0;
  if ($4 >>> 0 < $2 >>> 0) {
   $1 = $1 + 1 | 0
  }
  $4 = $4 ^ $10;
  $8 = $1 ^ $11;
  $5 = __wasm_rotl_i64($4, $8, 17);
  $10 = i64toi32_i32$HIGH_BITS;
  $2 = __wasm_rotl_i64($6, $7, 21) ^ $2;
  $9 = $9 ^ i64toi32_i32$HIGH_BITS;
  $0 = __wasm_rotl_i64($3, $0, 32);
  $1 = $0 + $2 | 0;
  $3 = i64toi32_i32$HIGH_BITS + $9 | 0;
  $6 = $1 >>> 0 < $0 >>> 0 ? $3 + 1 | 0 : $3;
  $0 = $8 + $6 | 0;
  $3 = $1 + $4 | 0;
  if ($3 >>> 0 < $1 >>> 0) {
   $0 = $0 + 1 | 0
  }
  $1 = __wasm_rotl_i64(__wasm_rotl_i64($2, $9, 16) ^ $1, $6 ^ i64toi32_i32$HIGH_BITS, 21) ^ ($3 ^ $5);
  $2 = i64toi32_i32$HIGH_BITS ^ ($0 ^ $10);
  $0 = __wasm_rotl_i64($3, $0, 32) ^ $1;
  i64toi32_i32$HIGH_BITS = i64toi32_i32$HIGH_BITS ^ $2;
  return $0;
 }
 
 function dump_stack_print_info() {
  var $0 = 0, $1 = 0, $2 = 0, $3 = 0, $4 = 0;
  $0 = global$0 + -64 | 0;
  global$0 = $0;
  $1 = HEAP32[2];
  $2 = HEAP32[$1 + 496 >> 2];
  $3 = print_tainted();
  $4 = strcspn();
  HEAP32[$0 + 48 >> 2] = 3291;
  HEAP32[$0 + 44 >> 2] = $4;
  HEAP32[$0 + 40 >> 2] = 3226;
  HEAP32[$0 + 36 >> 2] = $3;
  HEAP32[$0 + 32 >> 2] = 32753;
  HEAP32[$0 + 28 >> 2] = $1 + 700;
  HEAP32[$0 + 24 >> 2] = $2;
  HEAP32[$0 + 20 >> 2] = 0;
  HEAP32[$0 + 16 >> 2] = 32777;
  printk(32709, $0 + 16 | 0);
  if (HEAPU8[139904]) {
   HEAP32[$0 + 4 >> 2] = 139904;
   HEAP32[$0 >> 2] = 32777;
   printk(32756, $0);
  }
  print_worker_info(HEAP32[2]);
  global$0 = $0 - -64 | 0;
 }
 
 function dump_stack() {
  dump_stack_print_info();
  printk(31005, 0);
  if (!HEAPU8[82868]) {
   HEAP8[82868] = 1;
   printk(30960, 0);
  }
 }
 
 function memmove($0, $1, $2) {
  label$1 : {
   if ($2) {
    if ($0 >>> 0 <= $1 >>> 0) {
     break label$1
    }
    $1 = $1 + -1 | 0;
    while (1) {
     HEAP8[($0 + $2 | 0) + -1 | 0] = HEAPU8[$1 + $2 | 0];
     $2 = $2 + -1 | 0;
     if ($2) {
      continue
     }
     break;
    };
   }
   return;
  }
  memcpy($0, $1, $2);
 }
 
 function _ZN17compiler_builtins3int3mul3Mul3mul17h070e9a1c69faec5bE($0, $1, $2, $3) {
  var $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0;
  $4 = $2 >>> 16 | 0;
  $5 = $0 >>> 16 | 0;
  $9 = Math_imul($4, $5);
  $6 = $2 & 65535;
  $7 = $0 & 65535;
  $8 = Math_imul($6, $7);
  $5 = ($8 >>> 16 | 0) + Math_imul($5, $6) | 0;
  $4 = ($5 & 65535) + Math_imul($4, $7) | 0;
  $0 = (Math_imul($1, $2) + $9 | 0) + Math_imul($0, $3) + ($5 >>> 16) + ($4 >>> 16) | 0;
  $1 = $8 & 65535 | $4 << 16;
  i64toi32_i32$HIGH_BITS = $0;
  return $1;
 }
 
 function __wasm_i64_mul($0, $1, $2, $3) {
  $0 = _ZN17compiler_builtins3int3mul3Mul3mul17h070e9a1c69faec5bE($0, $1, $2, $3);
  return $0;
 }
 
 function __wasm_rotl_i32($0, $1) {
  var $2 = 0, $3 = 0;
  $2 = $1 & 31;
  $3 = (-1 >>> $2 & $0) << $2;
  $2 = $0;
  $0 = 0 - $1 & 31;
  return $3 | ($2 & -1 << $0) >>> $0;
 }
 
 function __wasm_rotl_i64($0, $1, $2) {
  var $3 = 0, $4 = 0, $5 = 0, $6 = 0;
  $6 = $2 & 63;
  $5 = $6;
  $3 = $5 & 31;
  if (32 <= $5 >>> 0) {
   $3 = -1 >>> $3 | 0
  } else {
   $4 = -1 >>> $3 | 0;
   $3 = (1 << $3) - 1 << 32 - $3 | -1 >>> $3;
  }
  $5 = $3 & $0;
  $3 = $1 & $4;
  $4 = $6 & 31;
  if (32 <= $6 >>> 0) {
   $3 = $5 << $4;
   $6 = 0;
  } else {
   $3 = (1 << $4) - 1 & $5 >>> 32 - $4 | $3 << $4;
   $6 = $5 << $4;
  }
  $5 = $3;
  $4 = 0 - $2 & 63;
  $3 = $4;
  $2 = $3 & 31;
  if (32 <= $3 >>> 0) {
   $3 = -1 << $2;
   $2 = 0;
  } else {
   $3 = (1 << $2) - 1 & -1 >>> 32 - $2 | -1 << $2;
   $2 = -1 << $2;
  }
  $0 = $2 & $0;
  $3 = $1 & $3;
  $1 = $4 & 31;
  if (32 <= $4 >>> 0) {
   $2 = 0;
   $0 = $3 >>> $1 | 0;
  } else {
   $2 = $3 >>> $1 | 0;
   $0 = ((1 << $1) - 1 & $3) << 32 - $1 | $0 >>> $1;
  }
  $0 = $0 | $6;
  i64toi32_i32$HIGH_BITS = $2 | $5;
  return $0;
 }
 
 var FUNCTION_TABLE = [null, do_no_restart_syscall, do_early_param, unknown_bootoption, set_init_arg, put_cred_rcu, wq_barrier_func, tasklet_action, tasklet_hi_action, no_blink, mmdrop_async_fn, process_timeout, run_timer_softirq, hrtimer_run_softirq, ktime_get, ktime_get_real, ktime_get_boottime, ktime_get_clocktai, dummy_clock_read, jiffies_read, rcu_process_callbacks, wake_up_klogd_work_func, __printk_safe_flush, dequeue_task_idle, check_preempt_curr_idle, pick_next_task_idle, put_prev_task_idle, arch_release_task_struct, task_tick_idle, switched_to_idle, prio_changed_idle, get_rr_interval_idle, arch_release_task_struct, enqueue_task_fair, dequeue_task_fair, yield_task_fair, yield_to_task_fair, check_preempt_wakeup, pick_next_task_fair, put_prev_task_fair, set_curr_task_fair, task_tick_fair, task_fork_fair, switched_from_fair, switched_to_fair, prio_changed_fair, get_rr_interval_fair, update_curr_fair, sched_rt_period_timer, enqueue_task_rt, dequeue_task_rt, yield_task_rt, check_preempt_curr_rt, pick_next_task_rt, put_prev_task_rt, set_curr_task_rt, task_tick_rt, switched_to_rt, prio_changed_rt, get_rr_interval_rt, update_curr_rt, dl_task_timer, inactive_task_timer, enqueue_task_dl, dequeue_task_dl, yield_task_dl, check_preempt_curr_dl, pick_next_task_dl, put_prev_task_dl, set_curr_task_rt, task_tick_dl, arch_release_task_struct, switched_from_dl, switched_to_dl, prio_changed_dl, update_curr_dl, autoremove_wake_function, default_wake_function, kmem_rcu_free, free_work, page_lock_anon_vma_read, page_referenced_one, get_rr_interval_idle, invalid_mkclean_vma, page_mkclean_one, page_mapcount_is_zero, try_to_unmap_one, invalid_migration_vma, drain_local_pages_wq, page_alloc_cpu_dead, free_compound_page, workingset_update_node, wake_page_function, do_write, do_run, do_shutdown, sys_ni_syscall, sys_m_read, sys_m_ioctl, sys_m_writev, put_prev_task_idle, put_prev_task_idle, put_prev_task_idle, u32_swap, generic_swap, u64_swap, radix_tree_node_rcu_free, cmp_ex_sort];
 function __wasm_memory_size() {
  return buffer.byteLength / 65536 | 0;
 }
 
 function __wasm_memory_grow(pagesToAdd) {
  pagesToAdd = pagesToAdd | 0;
  var oldPages = __wasm_memory_size() | 0;
  var newPages = oldPages + pagesToAdd | 0;
  if ((oldPages < newPages) && (newPages < 65536)) {
   var newBuffer = new ArrayBuffer(Math_imul(newPages, 65536));
   var newHEAP8 = new global.Int8Array(newBuffer);
   newHEAP8.set(HEAP8);
   HEAP8 = newHEAP8;
   HEAP8 = new global.Int8Array(newBuffer);
   HEAP16 = new global.Int16Array(newBuffer);
   HEAP32 = new global.Int32Array(newBuffer);
   HEAPU8 = new global.Uint8Array(newBuffer);
   HEAPU16 = new global.Uint16Array(newBuffer);
   HEAPU32 = new global.Uint32Array(newBuffer);
   HEAPF32 = new global.Float32Array(newBuffer);
   HEAPF64 = new global.Float64Array(newBuffer);
   buffer = newBuffer;
  }
  return oldPages;
 }
 
 return {
  "memory": Object.create(Object.prototype, {
   "grow": {
    "value": __wasm_memory_grow
   }, 
   "buffer": {
    "get": function () {
     return buffer;
    }
    
   }
  }), 
  "start_kernel": start_kernel, 
  "evt_loop": evt_loop, 
  "__syscall0": __syscall0, 
  "__syscall1": __syscall1, 
  "__syscall2": __syscall2, 
  "__syscall3": __syscall3, 
  "__syscall4": __syscall4, 
  "__syscall5": __syscall5, 
  "__syscall6": __syscall6
 };
}

var memasmFunc = new ArrayBuffer(262144);
for (var base64ReverseLookup = new Uint8Array(123/*'z'+1*/), i = 25; i >= 0; --i) {
    base64ReverseLookup[48+i] = 52+i; // '0-9'
    base64ReverseLookup[65+i] = i; // 'A-Z'
    base64ReverseLookup[97+i] = 26+i; // 'a-z'
  }
  base64ReverseLookup[43] = 62; // '+'
  base64ReverseLookup[47] = 63; // '/'
  /** @noinline Inlining this function would mean expanding the base64 string 4x times in the source code, which Closure seems to be happy to do. */
  function base64DecodeToExistingUint8Array(uint8Array, offset, b64) {
    var b1, b2, i = 0, j = offset, bLength = b64.length, end = offset + (bLength*3>>2);
    if (b64[bLength-2] == '=') --end;
    if (b64[bLength-1] == '=') --end;
    for (; i < bLength; i += 4, j += 3) {
      b1 = base64ReverseLookup[b64.charCodeAt(i+1)];
      b2 = base64ReverseLookup[b64.charCodeAt(i+2)];
      uint8Array[j] = base64ReverseLookup[b64.charCodeAt(i)] << 2 | b1 >> 4;
      if (j+1 < end) uint8Array[j+1] = b1 << 4 | b2 >> 2;
      if (j+2 < end) uint8Array[j+2] = b2 << 6 | base64ReverseLookup[b64.charCodeAt(i+3)];
    }
  }
var bufferView = new Uint8Array(memasmFunc);
base64DecodeToExistingUint8Array(bufferView, 1028, "KEQBAAIAAAAAACA=");
base64DecodeToExistingUint8Array(bufferView, 1048, "eAAAAHgAAAB4");
base64DecodeToExistingUint8Array(bufferView, 1096, "SAQAAEgE");
base64DecodeToExistingUint8Array(bufferView, 1152, "gAQAAIAE");
base64DecodeToExistingUint8Array(bufferView, 1168, "GQ==");
base64DecodeToExistingUint8Array(bufferView, 1364, "AQAAAAEAAABcBQAAXAUAAAAAAAB4PQ==");
base64DecodeToExistingUint8Array(bufferView, 1480, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 1529, "BAAAAAQAAAAGAAAABgAACAYAAAgGAAAABAAAFAYAABQGAAAcBgAAHAYAAAgP");
base64DecodeToExistingUint8Array(bufferView, 1608, "SAYAAEgGAACUBwAAlAc=");
base64DecodeToExistingUint8Array(bufferView, 1716, "KA4AACgOAABzd2FwcGVy");
base64DecodeToExistingUint8Array(bufferView, 1745, "SwAAIEsAAIwOAACIBwAACAk=");
base64DecodeToExistingUint8Array(bufferView, 1776, "8AYAAPAG");
base64DecodeToExistingUint8Array(bufferView, 1888, "UMM=");
base64DecodeToExistingUint8Array(bufferView, 1912, "KEQC");
base64DecodeToExistingUint8Array(bufferView, 1936, "AQAAAFAGAABQBgAAnAcAAJwHAAAAAAAAqAcAAKgH");
base64DecodeToExistingUint8Array(bufferView, 2000, "CA8AAAgPAAAIDwAACA8=");
base64DecodeToExistingUint8Array(bufferView, 2160, "////////////////////////////////AACAAP////8AAAAA////////////////");
base64DecodeToExistingUint8Array(bufferView, 2217, "BAAAABAAAAAAAQAAAAEA/////////////////////w==");
base64DecodeToExistingUint8Array(bufferView, 2257, "gAwAAIAM");
base64DecodeToExistingUint8Array(bufferView, 2280, "//////////8=");
base64DecodeToExistingUint8Array(bufferView, 2305, "CQAAAAkAAAE=");
base64DecodeToExistingUint8Array(bufferView, 3084, "DAwAAAwMAAACAAAATGludXg=");
base64DecodeToExistingUint8Array(bufferView, 3161, "bWV0YWw=");
base64DecodeToExistingUint8Array(bufferView, 3226, "dXRzX3JlbGVhc2U=");
base64DecodeToExistingUint8Array(bufferView, 3291, "dXRzX3ZlcnNpb24=");
base64DecodeToExistingUint8Array(bufferView, 3356, "dXRzX21hY2hpbmU=");
base64DecodeToExistingUint8Array(bufferView, 3421, "KG5vbmUp");
base64DecodeToExistingUint8Array(bufferView, 3488, "WA8=");
base64DecodeToExistingUint8Array(bufferView, 3504, "/v//7w==");
base64DecodeToExistingUint8Array(bufferView, 3520, "G04AACJO");
base64DecodeToExistingUint8Array(bufferView, 3568, "504=");
base64DecodeToExistingUint8Array(bufferView, 3616, "AgAAAAAAAAAE");
base64DecodeToExistingUint8Array(bufferView, 3672, "/////z8AAAD/////PwAAAP////8/");
base64DecodeToExistingUint8Array(bufferView, 3704, "bBAAAFgPAAAgDg==");
base64DecodeToExistingUint8Array(bufferView, 3724, "AQAAABQM");
base64DecodeToExistingUint8Array(bufferView, 3740, "qA4=");
base64DecodeToExistingUint8Array(bufferView, 3752, "AgAAAAQAgA==");
base64DecodeToExistingUint8Array(bufferView, 3783, "gAAE");
base64DecodeToExistingUint8Array(bufferView, 3800, "WA8=");
base64DecodeToExistingUint8Array(bufferView, 3844, "/P//7wE=");
base64DecodeToExistingUint8Array(bufferView, 3884, "qA4AAAQAgA==");
base64DecodeToExistingUint8Array(bufferView, 3904, "4gQAAAo=");
base64DecodeToExistingUint8Array(bufferView, 3928, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 3940, "/////w==");
base64DecodeToExistingUint8Array(bufferView, 3992, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 4004, "/////w==");
base64DecodeToExistingUint8Array(bufferView, 4056, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 4068, "/////w==");
base64DecodeToExistingUint8Array(bufferView, 4120, "Aw==");
base64DecodeToExistingUint8Array(bufferView, 4148, "/f//7wE=");
base64DecodeToExistingUint8Array(bufferView, 4204, "AQAAAAE=");
base64DecodeToExistingUint8Array(bufferView, 4504, "mBEAAJgRAAC/Ug==");
base64DecodeToExistingUint8Array(bufferView, 8472, "x1I=");
base64DecodeToExistingUint8Array(bufferView, 8496, "ECcAAP////8=");
base64DecodeToExistingUint8Array(bufferView, 11064, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 11084, "Dg==");
base64DecodeToExistingUint8Array(bufferView, 11100, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 11124, "Dw==");
base64DecodeToExistingUint8Array(bufferView, 11140, "AgAAAAc=");
base64DecodeToExistingUint8Array(bufferView, 11164, "EA==");
base64DecodeToExistingUint8Array(bufferView, 11180, "AwAAAAs=");
base64DecodeToExistingUint8Array(bufferView, 11204, "EQ==");
base64DecodeToExistingUint8Array(bufferView, 11220, "BAAAAAE=");
base64DecodeToExistingUint8Array(bufferView, 11244, "Dg==");
base64DecodeToExistingUint8Array(bufferView, 11260, "BQ==");
base64DecodeToExistingUint8Array(bufferView, 11284, "Dw==");
base64DecodeToExistingUint8Array(bufferView, 11300, "BgAAAAc=");
base64DecodeToExistingUint8Array(bufferView, 11324, "EA==");
base64DecodeToExistingUint8Array(bufferView, 11340, "BwAAAAs=");
base64DecodeToExistingUint8Array(bufferView, 11364, "EQ==");
base64DecodeToExistingUint8Array(bufferView, 11400, "eC0=");
base64DecodeToExistingUint8Array(bufferView, 11456, "eC0=");
base64DecodeToExistingUint8Array(bufferView, 11528, "eC0=");
base64DecodeToExistingUint8Array(bufferView, 11584, "eC0=");
base64DecodeToExistingUint8Array(bufferView, 11640, "Eg==");
base64DecodeToExistingUint8Array(bufferView, 11736, "0QMAAEAAAAAAJPQAACT0AP////////9/EwAAAAAAAAD/////AAAAAAAACT0I");
base64DecodeToExistingUint8Array(bufferView, 11800, "CgAAAAAAAABWVQ==");
base64DecodeToExistingUint8Array(bufferView, 11820, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 11860, "UC4AAFAuAAAAAAAAAQAAAAEAAAABAAAAAQAAAHAuAABwLgAAaAcBAAACAAABAAAAhC4AAIQuAAAAAAAABAAAAAAAAAAVAAAAnC4AAJwu");
base64DecodeToExistingUint8Array(bufferView, 12200, "QEIPAPB+Dg==");
base64DecodeToExistingUint8Array(bufferView, 13432, "gI1bALBxCwBAQg8AGQ==");
base64DecodeToExistingUint8Array(bufferView, 15620, "LAAAAIAAAAAAAAQ=");
base64DecodeToExistingUint8Array(bufferView, 15640, "Pls=");
base64DecodeToExistingUint8Array(bufferView, 15660, "LD0AACw9AAA0PQAAND0AADw9AAA8PQAAAAAAAP////8BAAAAgAAAAAAAAABgNgEAXlsAAAEAAACAAAAAAAAAAGA8AQBlWw==");
base64DecodeToExistingUint8Array(bufferView, 15774, "AgACAAAAAQ==");
base64DecodeToExistingUint8Array(bufferView, 15796, "tD0AALQ9AAC8PQAAvD0=");
base64DecodeToExistingUint8Array(bufferView, 16108, "WA8=");
base64DecodeToExistingUint8Array(bufferView, 16144, "ED8AABA/");
base64DecodeToExistingUint8Array(bufferView, 16208, "LmYAAAAAAAAH");
base64DecodeToExistingUint8Array(bufferView, 16496, "cEAAAHBA");
base64DecodeToExistingUint8Array(bufferView, 16512, "gEAAAIBAAACIQAAAiEAAAAAAAACUQAAAlEAAAJxAAACcQAAA4gQAAAo=");
base64DecodeToExistingUint8Array(bufferView, 16844, "FAAAAAAAAADUQQAA1EEAAAAAAAABAAAAAQAAAAEAAAABAAAAAQAAAD//Hw==");
base64DecodeToExistingUint8Array(bufferView, 16972, "4gQAAAo=");
base64DecodeToExistingUint8Array(bufferView, 16996, "+gAAAAE=");
base64DecodeToExistingUint8Array(bufferView, 17020, "AQAAAAAAAACEQgAAhEIAAAAAAACQQgAAkEIAAAAAAACcQgAAnEIAAOIEAAAK");
base64DecodeToExistingUint8Array(bufferView, 19200, "AQAAAAAAAAAS");
base64DecodeToExistingUint8Array(bufferView, 19232, "AQ==");
base64DecodeToExistingUint8Array(bufferView, 19248, "NEsAACAAAABgSwAAVEsAAFhLAABcSw==");
base64DecodeToExistingUint8Array(bufferView, 19424, "BAAAAEAAAABAAAAAACAAAEEAAACkQAAAAIAAAAAgAAAwdQAA0A8AAKAf");
base64DecodeToExistingUint8Array(bufferView, 19600, "SEQB");
base64DecodeToExistingUint8Array(bufferView, 19744, "UEgB");
base64DecodeToExistingUint8Array(bufferView, 19760, "AQAAAAAAAMAAAQAAAQAAAIBNAABgIAIAoHw=");
base64DecodeToExistingUint8Array(bufferView, 19836, "AQAAAAwAAACAAAAAAAIAAAAQAAAAgAAAaAAAAEwAAAAzAAAAGQAAAAEAAAAKAAAAIAAAAIAAAAAABAAAACAAABoAAAATAAAADgAAAAcAAAABAAAAAAAAAAE=");
base64DecodeToExistingUint8Array(bufferView, 19936, "TGludXggdmVyc2lvbiB1dHNfcmVsZWFzZSAocUBPcGVuQlNEKSAoQ0xhbmcpIHV0c192ZXJzaW9uCgBIT01FPS8AVEVSTT1saW51eABlYXJseSBvcHRpb25zAAE1JXMAATNTa2lwcGluZyBzZXR1cF9jb21tYW5kX2xpbmUgYW5kIG90aGVyIGZ1bmN0aW9ucyB0aGF0IGRlcGVuZCBvbiB3b3JraW5nIG1tCgABNUtlcm5lbCBjb21tYW5kIGxpbmU6ICVzCgBCb290aW5nIGtlcm5lbABTZXR0aW5nIGluaXQgYXJncwBUb28gbWFueSBib290ICVzIHZhcnMgYXQgYCVzJwBpbml0AGNvbnNvbGUAZWFybHljb24AATRNYWxmb3JtZWQgZWFybHkgb3B0aW9uICclcycKAGVudgABNEJVRzogZmFpbHVyZSBhdCAlczolZC8lcygpIQoAbWFpbi5jAHJlcGFpcl9lbnZfc3RyaW5nAAE0UGFyYW1ldGVyICVzIGlzIG9ic29sZXRlLCBpZ25vcmVkCgBwdXJlAGNvcmUAcG9zdGNvcmUAYXJjaABzdWJzeXMAZnMAZGV2aWNlAGxhdGUAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAGthbGxzeW1zLmMAZ2V0X3N5bWJvbF9wb3MAMHglbHgAKyUjbHgvJSNseAAgWyVzXQABNEJVRzogZmFpbHVyZSBhdCAlczolZC8lcygpIQoAY3JlZC5jAF9fcHV0X2NyZWQAQ1JFRDogcHV0X2NyZWRfcmN1KCkgc2VlcyAlcCB3aXRoIHVzYWdlICVkCgBNZXRhbABwYXJzZV9hcmdzAHBhcmFtcy5jAGRvaW5nICVzLCBwYXJzaW5nIEFSR1M6ICclcycKAC0tAAEzJXM6IFVua25vd24gcGFyYW1ldGVyIGAlcycKAAEzJXM6IGAlcycgdG9vIGxhcmdlIGZvciBwYXJhbWV0ZXIgYCVzJwoAAAEzJXM6IGAlcycgaW52YWxpZCBmb3IgcGFyYW1ldGVyIGAlcycKAHBhcnNlX29uZQBoYW5kbGluZyAlcyB3aXRoICVwCgBkb2luZyAlczogJXM9JyVzJwoAVW5rbm93biBhcmd1bWVudCAnJXMnCgABNVNldHRpbmcgZGFuZ2Vyb3VzIG9wdGlvbiAlcyAtIHRhaW50aW5nIGtlcm5lbAoAATVTb3J0aW5nIF9fZXhfdGFibGUuLi4KACVzV29ya3F1ZXVlOiAlcyAlcGYAAWMgKCVzKQABYwoAATR3b3JrcXVldWU6IHJvdW5kLXJvYmluIENQVSBzZWxlY3Rpb24gZm9yY2VkLCBleHBlY3QgcGVyZm9ybWFuY2UgaW1wYWN0CgAmeC0+d2FpdABwcmludF9kcm9wcGVkX3NpZ25hbAABNiVzLyVkOiByZWFjaGVkIFJMSU1JVF9TSUdQRU5ESU5HLCBkcm9wcGVkIHNpZ25hbCAlZAoAATRNZXRhbDogQlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBzb2Z0aXJxLmMAdGFza2xldF9hY3Rpb25fY29tbW9uAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBjcHUuYwBvZmZsaW5lAG9ubGluZQBjcHVocF9pc3N1ZV9jYWxsAAEwS2VybmVsIHBhbmljIC0gbm90IHN5bmNpbmc6ICVzCgABMFJlYm9vdGluZyBpbiAlZCBzZWNvbmRzLi4KAAEwLS0tWyBlbmQgS2VybmVsIHBhbmljIC0gbm90IHN5bmNpbmc6ICVzIF0tLS0KAAAAUEcBRiABUyAAUiAATSAAQiAAVSAARCAAQSAAVyAAQyABSSAATyABRSABTCAASyABWCABVCABVGFpbnRlZDogAE5vdCB0YWludGVkAAE0RGlzYWJsaW5nIGxvY2sgZGVidWdnaW5nIGR1ZSB0byBrZXJuZWwgdGFpbnQKAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBmb3JrLmMAX19tbWRyb3AAATFCVUc6IEJhZCByc3MtY291bnRlciBzdGF0ZSBtbTolcCBpZHg6JWQgdmFsOiVsZAoAATFCVUc6IG5vbi16ZXJvIHBndGFibGVzX2J5dGVzIG9uIGZyZWVpbmcgbW06ICVsZAoAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAHRpbWVyLmMAYWRkX3RpbWVyAGFkZF90aW1lcl9vbgABM3NjaGVkdWxlX3RpbWVvdXQ6IHdyb25nIHRpbWVvdXQgdmFsdWUgJWx4CgBfX21vZF90aW1lcg==");
base64DecodeToExistingUint8Array(bufferView, 21728, "AQAAAAAAAAAIAAAACAAAAAgAAAAIAAAACAAAAAIAAAAIAAAACAAAAAgAAAADAAAACAAAAAgAAAAIAAAACAAAAJgFAQCgBQEAqAUBAAE0UGVyc2lzdGVudCBjbG9jayByZXR1cm5lZCBpbnZhbGlkIHZhbHVlAGppZmZpZXMAATNNZXRhbDogbG9nX2J1Zl9sZW46ICVsdSBieXRlcyBub3QgYXZhaWxhYmxlCgABNk1ldGFsOiBsb2dfYnVmX2xlbjogJXUgYnl0ZXMKAAE2TWV0YWw6IGVhcmx5IGxvZyBidWYgZnJlZTogJXUoJXUlJSkKACoqICVsbHUgcHJpbnRrIG1lc3NhZ2VzIGRyb3BwZWQgKioKADwldT4AWyU1bHUuMDAwMDAwXSAAWyU1bHUuJTA2bHVdIAA8dHJ1bmNhdGVkPgAKACV1LCVsbHUsJWxsdSwlYzsAXHglMDJ4AHByaW50a19zYWZlX2ZsdXNoOiBpbnRlcm5hbCBlcnJvcgoAJS4qcwABYwoATG9zdCAlZCBtZXNzYWdlKHMpIQoAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAGNvcmUuYwB3YWtlX3VwX3E=");
base64DecodeToExistingUint8Array(bufferView, 22208, "uVoBAEsYAQCj3AAAwbQAAMONAADicQAA1loAABFJAABlOgAAjC4AAEwlAADEHQAA1BcAACgTAABCDwAAMQwAAMUJAADHBwAAMgYAAP0EAAAABAAANAMAAI8CAAAOAgAApwEAAE8BAAAQAQAA1wAAAKwAAACJAAAAbgAAAFcAAABGAAAAOAAAAC0AAAAkAAAAHQAAABcAAAASAAAADwAAAAS9AADQ6QAACCkBAJJqAQBMzgEAeD8CAHrRAgDwgAMATGIEAPV/BQAl3QYAvJkIAF2+CgAhXQ0APscQAJf/FAA0NBoAh+ogAFJSKQD+UTMAAABAAAXsTwASDmQA2Zd8AHPumgA+ocMA8fDwAJDRMAH0BX0Bbl3eASXIUwKQSfEChDqoAyRJkgQFW7AFx3EcB7Dc0wgsZCELjuM4DhEREREBM0JVRzogc2NoZWR1bGluZyB3aGlsZSBhdG9taWM6ICVzLyVkLzB4JTA4eAoAc2NoZWR1bGluZyB3aGlsZSBhdG9taWMKAHBpY2tfbmV4dF90YXNr");
base64DecodeToExistingUint8Array(bufferView, 22624, "Fw==");
base64DecodeToExistingUint8Array(bufferView, 22636, "GAAAABkAAAAaAAAAGwAAABw=");
base64DecodeToExistingUint8Array(bufferView, 22668, "HQAAAB4AAAAfAAAAIAAAAAEzYmFkOiBzY2hlZHVsaW5nIGZyb20gdGhlIGlkbGUgdGhyZWFkIQoAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAGlkbGUuYwBzd2l0Y2hlZF90b19pZGxlAHByaW9fY2hhbmdlZF9pZGxlAAAAWFgAACEAAAAiAAAAIwAAACQAAAAlAAAAJgAAACcAAAAoAAAAKQAAACoAAAAAAAAAKwAAACwAAAAtAAAALgAAAC8AAAAQWQAAMQAAADIAAAAzAAAAAAAAADQAAAA1AAAANgAAADcAAAA4");
base64DecodeToExistingUint8Array(bufferView, 22920, "OQAAADoAAAA7AAAAPAAAAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBydC5jAGRlcXVldWVfdG9wX3J0X3JxAF9waWNrX25leHRfdGFza19ydABwaWNrX25leHRfcnRfZW50aXR5AHNjaGVkOiBSVCB0aHJvdHRsaW5nIGFjdGl2YXRlZAoAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAGRlYWRsaW5lLmMAAFRZAAA/AAAAQAAAAEEAAAAAAAAAQgAAAEMAAABEAAAARQAAAEYAAABHAAAAAAAAAEgAAABJAAAASgAAAAAAAABLAAAAcmVwbGVuaXNoX2RsX2VudGl0eQBzY2hlZDogREwgcmVwbGVuaXNoIGxhZ2dlZCB0b28gbXVjaAoAZW5xdWV1ZV90YXNrX2RsAGVucXVldWVfZGxfZW50aXR5AF9fZW5xdWV1ZV9kbF9lbnRpdHkAcGlja19uZXh0X3Rhc2tfZGwAYml0X3dhaXRfdGFibGUgKyBpAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBzbG9iLmMAa21lbV9jYWNoZQBzbG9iX2FsbG9jAHNsb2JfZnJlZQBtZW1vcnkAcmVzZXJ2ZWQAATRDb3VsZCBub3QgYWxsb2NhdGUgJXBhcCBieXRlcyBvZiBtaXJyb3JlZCBtZW1vcnkKAAE2ICAgbWVtYmxvY2tfZnJlZTogWyVwYS0lcGFdICVwRgoAATZtZW1ibG9ja19yZXNlcnZlOiBbJXBhLSVwYV0gJXBGCgABNiVzOiAlbGx1IGJ5dGVzIGFsaWduPTB4JWxseCBuaWQ9JWQgZnJvbT0lcGEgbWF4X2FkZHI9JXBhICVwRgoAbWVtYmxvY2tfYWxsb2NfdHJ5X25pZF9ub3BhbmljAG1lbWJsb2NrX2FsbG9jX3RyeV9uaWQAJXM6IEZhaWxlZCB0byBhbGxvY2F0ZSAlbGx1IGJ5dGVzIGFsaWduPTB4JWxseCBuaWQ9JWQgZnJvbT0lcGEgbWF4X2FkZHI9JXBhCgABNk1FTUJMT0NLIGNvbmZpZ3VyYXRpb246CgABNiBtZW1vcnkgc2l6ZSA9ICVwYSByZXNlcnZlZCBzaXplID0gJXBhCgABNEJVRzogZmFpbHVyZSBhdCAlczolZC8lcygpIQoAbWVtYmxvY2suYwBtZW1ibG9ja19pbnNlcnRfcmVnaW9uAAEzbWVtYmxvY2s6IEZhaWxlZCB0byBkb3VibGUgJXMgYXJyYXkgZnJvbSAlbGQgdG8gJWxkIGVudHJpZXMgIQoAATZtZW1ibG9jazogJXMgaXMgZG91YmxlZCB0byAlbGQgYXQgWyVwYS0lcGFdAG1lbWJsb2NrX2RvdWJsZV9hcnJheQBtZW1ibG9ja19tZXJnZV9yZWdpb25zAAEzJXMgaXMgYnJlYWtpbmcKAG1lbWJsb2NrX2FsbG9jX2ludGVybmFsAAE2ICVzLmNudCAgPSAweCVseAoAATYgJXNbJSN4XQlbJXBhLSVwYV0sICVwYSBieXRlcyVzIGZsYWdzOiAlI3gKAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgB2bWFsbG9jLmMAdmZyZWUAX19pbnNlcnRfdm1hcF9hcmVhAHZ1bm1hcF9wYWdlX3JhbmdlAF9fdnVubWFwAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBybWFwLmMAcGFnZV9ta2NsZWFuAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBtbG9jay5jAG1sb2NrX3ZtYV9wYWdlAGxvY2tlZAB3YWl0ZXJzAGVycm9yAHJlZmVyZW5jZWQAdXB0b2RhdGUAZGlydHkAbHJ1AGFjdGl2ZQB3b3JraW5nc2V0AHNsYWIAb3duZXJfcHJpdl8xAGFyY2hfMQByZXNlcnZlZABwcml2YXRlAHByaXZhdGVfMgB3cml0ZWJhY2sAaGVhZABtYXBwZWR0b2Rpc2sAcmVjbGFpbQBzd2FwYmFja2VkAHVuZXZpY3RhYmxlAG1sb2NrZWQAAAAAAAABAAAA/14AAIAAAAAGXwAAAAEAAA5fAAACAAAAFF8AAAQAAAAfXwAACAAAAChfAAAQAAAALl8AACAAAAAyXwAAQAAAADlfAAAAAgAARF8AAAAEAABJXwAAAAgAAFZfAAAAEAAAXV8AAAAgAABmXwAAAEAAAG5fAAAAgAAAeF8AAAAAAQCCXwAAAAACAIdfAAAAAAQAlF8AAAAACACcXwAAAAAQAKdfAAAAACAAs18=");
base64DecodeToExistingUint8Array(bufferView, 24696, "R0ZQX1RSQU5TSFVHRQBHRlBfVFJBTlNIVUdFX0xJR0hUAEdGUF9ISUdIVVNFUl9NT1ZBQkxFAEdGUF9ISUdIVVNFUgBHRlBfVVNFUgBHRlBfS0VSTkVMX0FDQ09VTlQAR0ZQX0tFUk5FTABHRlBfTk9GUwBHRlBfQVRPTUlDAEdGUF9OT0lPAEdGUF9OT1dBSVQAR0ZQX0RNQQBfX0dGUF9ISUdITUVNAEdGUF9ETUEzMgBfX0dGUF9ISUdIAF9fR0ZQX0FUT01JQwBfX0dGUF9JTwBfX0dGUF9GUwBfX0dGUF9OT1dBUk4AX19HRlBfUkVUUllfTUFZRkFJTABfX0dGUF9OT0ZBSUwAX19HRlBfTk9SRVRSWQBfX0dGUF9DT01QAF9fR0ZQX1pFUk8AX19HRlBfTk9NRU1BTExPQwBfX0dGUF9NRU1BTExPQwBfX0dGUF9IQVJEV0FMTABfX0dGUF9USElTTk9ERQBfX0dGUF9SRUNMQUlNQUJMRQBfX0dGUF9NT1ZBQkxFAF9fR0ZQX0FDQ09VTlQAX19HRlBfV1JJVEUAX19HRlBfUkVDTEFJTQBfX0dGUF9ESVJFQ1RfUkVDTEFJTQBfX0dGUF9LU1dBUERfUkVDTEFJTQ==");
base64DecodeToExistingUint8Array(bufferView, 25184, "ykIjAHhgAADKQgMAhmAAAMoAYgCaYAAAwgBiAK9gAADAAGIAvGAAAMAAcADFYAAAwABgANhgAABAAGAA42AAACAASADsYAAAAABgAPdgAAAAAEAAAGEAAAEAAAALYQAAAgAAABNhAAAEAAAAIWEAACAAAAArYQAAAAAIADZhAABAAAAAQ2EAAIAAAABMYQAAAAIAAFVhAAAABAAAYmEAAAAIAAB2YQAAABAAAINhAAAAQAAAkWEAAACAAACcYQAAAAABAKdhAAAAIAAAuGEAAAAAAgDHYQAAAAAEANZhAAAQAAAA5WEAAAgAAAD3YQAAAAAQAAViAAAAAQAAE2IAAAAAYAAfYgAAAAAgAC1iAAAAAEAAQmI=");
base64DecodeToExistingUint8Array(bufferView, 25472, "cmVhZAB3cml0ZQBleGVjAHNoYXJlZABtYXlyZWFkAG1heXdyaXRlAG1heWV4ZWMAbWF5c2hhcmUAZ3Jvd3Nkb3duAHVmZmRfbWlzc2luZwBwZm5tYXAAZGVueXdyaXRlAHVmZmRfd3AAaW8Ac2VxcmVhZAByYW5kcmVhZABkb250Y29weQBkb250ZXhwYW5kAGxvY2tvbmZhdWx0AGFjY291bnQAbm9yZXNlcnZlAGh1Z2V0bGIAd2lwZW9uZm9yawBkb250ZHVtcABtaXhlZG1hcABodWdlcGFnZQBub2h1Z2VwYWdlAG1lcmdlYWJsZQ==");
base64DecodeToExistingUint8Array(bufferView, 25728, "AQAAAIBjAAACAAAAhWMAAAQAAACLYwAACAAAAJBjAAAQAAAAl2MAACAAAACfYwAAQAAAAKhjAACAAAAAsGMAAAABAAC5YwAAAAIAAMNjAAAABAAA0GMAAAAIAADXYwAAABAAAOFjAAAAIAAA/14AAABAAADpYwAAAIAAAOxjAAAAAAEA9GMAAAAAAgD9YwAAAAAEAAZkAAAAAAgAEWQAAAAAEAAdZAAAAAAgACVkAAAAAEAAL2QAAAAAAAFWXwAAAAAAAjdkAAAAAAAEQmQAAAAAABBLZAAAAAAAIFRkAAAAAABAXWQAAAAAAIBoZA==");
base64DecodeToExistingUint8Array(bufferView, 25976, "ATBwYWdlOiVweCBpcyB1bmluaXRpYWxpemVkIGFuZCBwb2lzb25lZAABMHBhZ2U6JXB4IGNvdW50OiVkIG1hcGNvdW50OiVkIG1hcHBpbmc6JXB4IGluZGV4OiUjbHgAAWMgY29tcG91bmRfbWFwY291bnQ6ICVkAAFjCgABMGZsYWdzOiAlI2x4KCVwR3ApCgABMQByYXc6IAABMXBhZ2UgZHVtcGVkIGJlY2F1c2U6ICVzCgBub29wAGlzb2xhdGVfbHJ1X3BhZ2UAATRNZXRhbDogQlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgB2bXNjYW4uYwABM01ldGFsOiBzaHJpbmtfc2xhYjogJXBGIG5lZ2F0aXZlIG9iamVjdHMgdG8gZGVsZXRlIG5yPSVsZAoAX19yZW1vdmVfbWFwcGluZwABNk1ldGFsOiAlczogb3JwaGFuZWQgcGFnZQoAcGFnZW91dAAAAAAg");
base64DecodeToExistingUint8Array(bufferView, 26360, "/////////38AAAAACAAAAGlzb2xhdGVfbHJ1X3BhZ2VzAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBwYWdlLXdyaXRlYmFjay5jAGNsZWFyX3BhZ2VfZGlydHlfZm9yX2lvAE1vdmFibGUAAAAAAAAAWg==");
base64DecodeToExistingUint8Array(bufferView, 26496, "AgAAAAEAAAAEAAAAAAAAAAIAAAAAAAAABA==");
base64DecodeToExistingUint8Array(bufferView, 26532, "AQAAAAQ=");
base64DecodeToExistingUint8Array(bufferView, 26560, "d2Fybl9hbGxvYwABNCVzOiAlcFYsIG1vZGU6JSN4KCVwR2cpLCBub2RlbWFzaz0lKnBibAoAYWN0aXZlX2Fub246JWx1IGluYWN0aXZlX2Fub246JWx1IGlzb2xhdGVkX2Fub246JWx1CiBhY3RpdmVfZmlsZTolbHUgaW5hY3RpdmVfZmlsZTolbHUgaXNvbGF0ZWRfZmlsZTolbHUKIHVuZXZpY3RhYmxlOiVsdSBkaXJ0eTolbHUgd3JpdGViYWNrOiVsdSB1bnN0YWJsZTolbHUKIHNsYWJfcmVjbGFpbWFibGU6JWx1IHNsYWJfdW5yZWNsYWltYWJsZTolbHUKIG1hcHBlZDolbHUgc2htZW06JWx1IHBhZ2V0YWJsZXM6JWx1IGJvdW5jZTolbHUKIGZyZWU6JWx1IGZyZWVfcGNwOiVsdSBmcmVlX2NtYTolbHUKAE5vZGUgJWQgYWN0aXZlX2Fub246JWx1a0IgaW5hY3RpdmVfYW5vbjolbHVrQiBhY3RpdmVfZmlsZTolbHVrQiBpbmFjdGl2ZV9maWxlOiVsdWtCIHVuZXZpY3RhYmxlOiVsdWtCIGlzb2xhdGVkKGFub24pOiVsdWtCIGlzb2xhdGVkKGZpbGUpOiVsdWtCIG1hcHBlZDolbHVrQiBkaXJ0eTolbHVrQiB3cml0ZWJhY2s6JWx1a0Igc2htZW06JWx1a0Igd3JpdGViYWNrX3RtcDolbHVrQiB1bnN0YWJsZTolbHVrQiBhbGxfdW5yZWNsYWltYWJsZT8gJXMKAHllcwBubwABYyVzIGZyZWU6JWx1a0IgbWluOiVsdWtCIGxvdzolbHVrQiBoaWdoOiVsdWtCIGFjdGl2ZV9hbm9uOiVsdWtCIGluYWN0aXZlX2Fub246JWx1a0IgYWN0aXZlX2ZpbGU6JWx1a0IgaW5hY3RpdmVfZmlsZTolbHVrQiB1bmV2aWN0YWJsZTolbHVrQiB3cml0ZXBlbmRpbmc6JWx1a0IgcHJlc2VudDolbHVrQiBtYW5hZ2VkOiVsdWtCIG1sb2NrZWQ6JWx1a0Iga2VybmVsX3N0YWNrOiVsdWtCIHBhZ2V0YWJsZXM6JWx1a0IgYm91bmNlOiVsdWtCIGZyZWVfcGNwOiVsdWtCIGxvY2FsX3BjcDoldWtCIGZyZWVfY21hOiVsdWtCCgBsb3dtZW1fcmVzZXJ2ZVtdOgABYyAlbGQAAWMKAAFjJXM6IAABYyVsdSolbHVrQiAAAWM9ICVsdWtCCgAlbGQgdG90YWwgcGFnZWNhY2hlIHBhZ2VzCgABNkJ1aWx0ICVpIHpvbmVsaXN0cywgbW9iaWxpdHkgZ3JvdXBpbmcgJXMuICBUb3RhbCBwYWdlczogJWxkCgBvZmYAb24AATZNZW1vcnk6ICVsdUsvJWx1SyBhdmFpbGFibGUgKCVsdUsga2VybmVsIGNvZGUsICVsdUsgcndkYXRhLCAlbHVLIHJvZGF0YSwgJWx1SyBpbml0LCAlbHVLIGJzcywgJWx1SyByZXNlcnZlZCwgJWx1SyBjbWEtcmVzZXJ2ZWQlcyVzKQoALCAAAG1tL3BhZ2VfYWxsb2M6ZGVhZABub256ZXJvIG1hcGNvdW50AG5vbi1OVUxMIG1hcHBpbmcAbm9uemVybyBfcmVmY291bnQAUEFHRV9GTEFHU19DSEVDS19BVF9GUkVFIGZsYWcocykgc2V0AAExQlVHOiBCYWQgcGFnZSBzdGF0ZTogJWx1IG1lc3NhZ2VzIHN1cHByZXNzZWQKAAExQlVHOiBCYWQgcGFnZSBzdGF0ZSBpbiBwcm9jZXNzICVzICBwZm46JTA1bHgKAAExYmFkIGJlY2F1c2Ugb2YgZmxhZ3M6ICUjbHgoJXBHcCkKAHdhcm5fYWxsb2Nfc2hvd19tZW0Abm9uemVybyBfY291bnQAUEFHRV9GTEFHU19DSEVDS19BVF9QUkVQIGZsYWcgc2V0AHBhZ2UgYWxsb2NhdGlvbiBmYWlsdXJlOiBvcmRlcjoldQABYyglcykgAAE3T24gbm9kZSAlZCB0b3RhbHBhZ2VzOiAlbHUKAAE3ICAlcyB6b25lOiAlbHUgcGFnZXMgdXNlZCBmb3IgbWVtbWFwCgAAlm4AAGJnAAABNCAgJXMgem9uZTogJWx1IHBhZ2VzIGV4Y2VlZHMgZnJlZXNpemUgJWx1CgABNyAgJXMgem9uZTogJWx1IHBhZ2VzIHJlc2VydmVkCgAmcGdkYXQtPmtzd2FwZF93YWl0ACZwZ2RhdC0+cGZtZW1hbGxvY193YWl0AE5vcm1hbAABNyAgJXMgem9uZTogJWx1IHBhZ2VzLCBMSUZPIGJhdGNoOiV1CgBPdXQgb2YgbWVtb3J5IChvb21fa2lsbF9hbGxvY2F0aW5nX3Rhc2spAAE0T3V0IG9mIG1lbW9yeSBhbmQgbm8ga2lsbGFibGUgcHJvY2Vzc2VzLi4uCgBTeXN0ZW0gaXMgZGVhZGxvY2tlZCBvbiBtZW1vcnkKAE91dCBvZiBtZW1vcnkATWVtb3J5IGNncm91cCBvdXQgb2YgbWVtb3J5AE91dCBvZiBtZW1vcnk6ICVzIHBhbmljX29uX29vbSBpcyBlbmFibGVkCgBjb21wdWxzb3J5AHN5c3RlbS13aWRlAG9vbV9raWxsX3Byb2Nlc3MAATMlczogS2lsbCBwcm9jZXNzICVkICglcykgc2NvcmUgJXUgb3Igc2FjcmlmaWNlIGNoaWxkCgABM0tpbGxlZCBwcm9jZXNzICVkICglcykgdG90YWwtdm06JWx1a0IsIGFub24tcnNzOiVsdWtCLCBmaWxlLXJzczolbHVrQiwgc2htZW0tcnNzOiVsdWtCCgABNm9vbSBraWxsZXIgJWQgKCVzKSBoYXMgbW0gcGlubmVkIGJ5ICVkICglcykKAAE0JXMgaW52b2tlZCBvb20ta2lsbGVyOiBnZnBfbWFzaz0lI3goJXBHZyksIG5vZGVtYXNrPSUqcGJsLCBvcmRlcj0lZCwgb29tX3Njb3JlX2Fkaj0laGQKAAE0Q09NUEFDVElPTiBpcyBkaXNhYmxlZCEhIQoAATZUYXNrcyBzdGF0ZSAobWVtb3J5IHZhbHVlcyBpbiBwYWdlcyk6CgABNlsgIHBpZCAgXSAgIHVpZCAgdGdpZCB0b3RhbF92bSAgICAgIHJzcyBwZ3RhYmxlc19ieXRlcyBzd2FwZW50cyBvb21fc2NvcmVfYWRqIG5hbWUKAAE2WyU3ZF0gJTVkICU1ZCAlOGx1ICU4bHUgJThsZCAlOGx1ICAgICAgICAgJTVoZCAlcwoAATRCVUc6IGZhaWx1cmUgYXQgJXM6JWQvJXMoKSEKAGZpbGVtYXAuYwB0cnlfdG9fcmVsZWFzZV9wYWdlAAExQlVHOiBCYWQgcGFnZSBjYWNoZSBpbiBwcm9jZXNzICVzICBwZm46JTA1bHgKAHN0aWxsIG1hcHBlZCB3aGVuIGRlbGV0ZWQAL2hvbWUvcXVpcmlucGEvd2FzbS9zcmMvbWV0YWwvaW5jbHVkZS9saW51eC94YXJyYXkuaAB4YXNfc2V0X29yZGVyAAEwV2ViYXNzZW1ibHkgY2FuJ3QganVtcCBpbnRvIGNvbXB1dGVkIGFkZHJlc3MKAAE0QlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgAvaG9tZS9xdWlyaW5wYS93YXNtL3NyYy9tZXRhbC9pbmNsdWRlL2xpbnV4L3NwaW5sb2NrLmgAc3Bpbl91bmxvY2tfYmg=");
base64DecodeToExistingUint8Array(bufferView, 29460, "XQ==");
base64DecodeToExistingUint8Array(bufferView, 29472, "XgAAAAAAAABfAAAAATNpb2N0bChkKVslZF0gKCVsdSwlbHUsJWx1KQoAAAAAAAAAYAAAAGAAAABgAAAAYQAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYgAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABjAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAGAAAABgAAAAYAAAAAEzbmlfc3lzY2FsbFslZF1bJWxkXSAoKQoAATNuaV9zeXNjYWxsWyVkXVslbGRdICglbHUpCgABM25pX3N5c2NhbGxbJWRdWyVsZF0gKCVsdSwlbHUpCgABM25pX3N5c2NhbGxbJWRdWyVsZF0gKCVsdSwlbHUsJWx1KQoAATNuaV9zeXNjYWxsWyVkXVslbGRdICglbHUsJWx1LCVsdSwlbHUpCgABM25pX3N5c2NhbGxbJWRdWyVsZF0gKCVsdSwlbHUsJWx1LCVsdSwlbHUpCgABM25pX3N5c2NhbGxbJWRdWyVsZF0gKCVsdSwlbHUsJWx1LCVsdSwlbHUsJWx1KQoAATRDT05GSUdfQVJDX0RXMl9VTldJTkQgbmVlZHMgdG8gYmUgZW5hYmxlZAoAATYKU3RhY2sgVHJhY2U6CgABMyVzOiBJbXBsZW1lbnQgbWUgcHJvcGVybHkuCgBzZXR1cF9wcm9jZXNzb3IAATMlczogSW1wbGVtZW50IG1lLgoAbWFjaGluZV9oYWx0AAEzJXM6IEltcGxlbWVudCBtZSBwcm9wZXJseS4KAG1hY2hpbmVfcmVzdGFydAABMyVzOiBJbXBsZW1lbnQgbWUuCgBhcmNfaW5pdF9JUlEAATZfX3N3aXRjaF90bwoAAAAAEEwAAKBMAAABMydzZXR1cF9hcmNoX21lbW9yeScgJXM6IEltcGxlbWVudCBtZSBwcm9wZXJseQpzdGFydF9jb2RlOiAlbHUKZW5kX2NvZGU6ICVsdQplbmRfZGF0YTogJWx1CmJyazogJWx1CgBzZXR1cF9hcmNoX21lbW9yeQABM21lbV9pbml0ICVzLgoAbWVtX2luaXQASS1DYWNoZQkJOiBOL0EKAEktQ2FjaGUJCTogJXVLLCAlZHdheS9zZXQsICV1QiBMaW5lLCAlcyVzJXMKAFZJUFQAUElQVAAgYWxpYXNpbmcAAChub3QgdXNlZCkgAEQtQ2FjaGUJCTogTi9BCgBELUNhY2hlCQk6ICV1SywgJWR3YXkvc2V0LCAldUIgTGluZSwgJXMlcyVzCgBTTEMJCTogJXVLLCAldUIgTGluZSVzCgBQZXJpcGhlcmFscwk6ICUjbHglcyVzCgABNiVzAAEzJXM6IEltcGxlbWVudCBtZS4AYXJjX21tdV9pbml0ACVzOiBub3QgaW1wbGVtZW50ZWQuCgB1dGxiX2ludmFsaWRhdGUAATRNZXRhbDolczogQlVHOiBmYWlsdXJlIGF0ICVzOiVkLyVzKCkhCgBfX2R5bmFtaWNfcHJfZGVidWcAZHluYW1pY19kZWJ1Zy5jAAE3JXMlcFYAPGludHI+IABbJWRdIAAlczoAJWQ6ACAAAAAAADAxMjM0NTY3ODlhYmNkZWY=");
base64DecodeToExistingUint8Array(bufferView, 31776, "MDEyMzQ1Njc4OUFCQ0RFRgAlcyUxNi4xNmxseAAgAAAlcyU4Ljh4ACVzJTQuNHgAJXMlcyVwOiAlcwoAJXMlcyUuOHg6ICVzCgAlcyVzJXMK");
base64DecodeToExistingUint8Array(bufferView, 31872, "AwIBAAUEBwYICQoLDA0ODwABAgMEBQYHCAkKCwwNDg9pbnB1dA==");
base64DecodeToExistingUint8Array(bufferView, 31924, "yCBuO5BB3HZYYbJNIIO47eij1tawwmSbeOIKoAgICAgICAgICCgoKCgoCAgICAgICAgICAgICAgICAgIoBAQEBAQEBAQEBAQEBAQEAQEBAQEBAQEBAQQEBAQEBAQQUFBQUFBAQEBAQEBAQEBAQEBAQEBAQEBAQEQEBAQEBBCQkJCQkICAgICAgICAgICAgICAgICAgICAhAQEBAI");
base64DecodeToExistingUint8Array(bufferView, 32112, "oBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBABAQEBAQEBAQEBAQEBAQEBAQEBAQEBARABAQEBAQEBAgICAgICAgICAgICAgICAgICAgICAgICEAICAgICAgICKG51bGwp");
base64DecodeToExistingUint8Array(bufferView, 32224, "MDAxMDIwMzA0MDUwNjA3MDgwOTAwMTExMjEzMTQxNTE2MTcxODE5MTAyMTIyMjMyNDI1MjYyNzI4MjkyMDMxMzIzMzM0MzUzNjM3MzgzOTMwNDE0MjQzNDQ0NTQ2NDc0ODQ5NDA1MTUyNTM1NDU1NTY1NzU4NTk1MDYxNjI2MzY0NjU2NjY3Njg2OTYwNzE3MjczNzQ3NTc2Nzc3ODc5NzA4MTgyODM4NDg1ODY4Nzg4ODk4MDkxOTI5Mzk0OTU5Njk3OTg5OTkoaW52YWxpZCBhZGRyZXNzKQAABgAAcBD//wAKAABwEP//AAIAADAQ//8A////AgAKAGlvICAAbWVtIABpcnEgAGRtYSAAYnVzIAA/Pz8gAHNpemUgACA2NGJpdAAgcHJlZgAgd2luZG93ACBkaXNhYmxlZAAgZmxhZ3MgAAAAAAAACv//AAAAAGAQ//9JNABwSy1lcnJvcgAAAP///wAA//8oIU9GKQAocHRydmFsKQABNCVzOiAlZCBjYWxsYmFja3Mgc3VwcHJlc3NlZAoATWVtLUluZm86CgAlbHUgcGFnZXMgUkFNCgAlbHUgcGFnZXMgSGlnaE1lbS9Nb3ZhYmxlT25seQoAJWx1IHBhZ2VzIHJlc2VydmVkCgAlc0NQVTogJWQgUElEOiAlZCBDb21tOiAlLjIwcyAlcyVzICVzICUuKnMKAAAgACVzSGFyZHdhcmUgbmFtZTogJXMKAAFk");
base64DecodeToExistingUint8Array(bufferView, 140036, "BCMCAAQjAg==");
base64DecodeToExistingUint8Array(bufferView, 141076, "FCcCABQnAgAAAAAAfU8AAIJPAACHTwAAkE8AAJVPAACcTwAAn08AAKZPAABERAEAREQBAEREAQBERAEAREQBAEREAQBERAEAREQBAEREAQAB");
base64DecodeToExistingUint8Array(bufferView, 141176, "WFAAAF5QAABpUAAAclAAALQAAAAAAAAAWFAAAAJRAABpUAAADFEAAJEAAAAAAAAAWFAAAAJRAABpUAAAIVEAAJsAAAAAAAAAWFAAAAJRAABpUAAANFEAAJ8=");
var retasmFunc = asmFunc({Math,Int8Array,Uint8Array,Int16Array,Uint16Array,Int32Array,Uint32Array,Float32Array,Float64Array,NaN,Infinity}, {abort:function() { throw new Error('abort'); },kmem,umem,flush,js_emem,js_shutdown,evt_count,js_run},memasmFunc);
export var memory = retasmFunc.memory;
export var start_kernel = retasmFunc.start_kernel;
export var evt_loop = retasmFunc.evt_loop;
export var __syscall0 = retasmFunc.__syscall0;
export var __syscall1 = retasmFunc.__syscall1;
export var __syscall2 = retasmFunc.__syscall2;
export var __syscall3 = retasmFunc.__syscall3;
export var __syscall4 = retasmFunc.__syscall4;
export var __syscall5 = retasmFunc.__syscall5;
export var __syscall6 = retasmFunc.__syscall6;
