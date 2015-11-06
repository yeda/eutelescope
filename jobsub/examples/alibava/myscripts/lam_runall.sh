#!/bin/bash

ALLRUNLIST=(1053 1057 1061 1062 1063 1064 1065 1066 1067 1068 1069 1070 1076 1077 1078 1079 1082 1083 1084 1085 1086 1087 1088 1089 1090 1091 1092 1223 1224 1225 1226 1227 1228 1229 1230 1231 1232 1233 1234 1235 1239 1240 1241 1242 1243 1244 1245 1246 1247 1248 1249 1250 1186 1187 1188 1189 1190 1191 1192 1194 1195 1196 1197 1198 1199 1205 1206 1207 1208 1209 1210 1211 1212 1213 1214 1215 1216 1148 1149 1150 1151 1152 1153 1154 1155 1157 1158 1159 1160 1161 1165 1166 1167 1168 1169 1170 1171 1172 1173 1174 1175 1176 1180 1103 1105 1106 1107 1108 1117 1118 1119 1120 1121 1122 1123 1124 1128 1129 1130 1131 1132 1133 1134 1135 1136 1137 1138 1139 1261 1262 2033 2034 2035 2036 2037 2039 2040 2041 2042 2043 2044 2045 2046 2052 2053 2054 2055 2056 2058 2059 2060 2061 2062 2064 2067 2071 2297 2334 2335 2336 2337 2338 2343 2344 2345 2192 2193 2194 2195 2196 2197 2203 2204 2206 2207 2208 2209 2210 2214 2215 2216 2217 2218 2219 2220 2221 2222 2223 2224 2225 2154 2155 2156 2157 2158 2159 2160 2161 2163 2164 2165 2166 2167 2168 2169 2173 2174 2175 2176 2177 2178 2179 2180 2181 2182 2319 2320 2321 2322 2324 2328 2329 2330 2331 2332 2333 2118 2120 2121 2122 2123 2124 2125 2126 2127 2128 2129 2130 2131 2135 2136 2137 2138 2140 2141 2142 2143 2144 2145 2146 2147 2236 2237 2238 2239 2240 2241 2243 2244 2245 2246 2247 2248 2249 2250 2255 2256 2258 2259 2260 2261 2262 2263 2264 2265 2266 2268 2081 2085 2086 2087 2088 2089 2090 2091 2092 2093 2094 2095 2096 2100 2101 2102 2103 2104 2105 2106 2107 2108 2109 2110 2111 2272 2273 2274 2275 2276 2281 2282 2283 2284 2285 2286 3044 3045 3046 3047 3048 3049 3050 3051 3052 3053 3054 3055 3056 3061 3062 3063 3064 3065 3066 3067 3068 3069 3070 3071 3072 3195 3196 3197 3198 3199 3200 3201 3214 3215 3216 3217 3218 3219 3220 3221 3222 3223 3224 3225 3226 3227 3231 3232 3233 3234 3157 3158 3159 3160 3161 3162 3163 3164 3165 3171 3172 3173 3174 3175 3176 3178 3179 3180 3181 3182 3183 3184 3185 3186 3187 3120 3121 3122 3123 3124 3125 3126 3127 3128 3129 3130 3131 3135 3136 3137 3138 3139 3140 3143 3144 3145 3146 3147 3148 3149 3080 3081 3082 3083 3084 3085 3086 3087 3088 3090 3091 3092 3093 3098 3099 3100 3101 3102 3103 3104 3106 3107 3108 3110 3111 4003 4004 4005 4006 4007 4008 4009 4010 4011 4012 4013 4014 4015 4021 4022 4023 4024 4025 4026 4027 4028 4029 4030 4031 4185 4186 4187 4188 4189 4190 4191 4192 4193 4194 4195 4196 4197 4201 4202 4203 4204 4205 4206 4207 4208 4209 4210 4211 4212 4151 4152 4153 4154 4155 4156 4157 4158 4159 4160 4161 4162 4163 4167 4168 4169 4170 4171 4172 4173 4174 4175 4176 4177 4113 4114 4115 4116 4117 4118 4119 4120 4125 4126 4127 4128 4129 4133 4134 4135 4136 4137 4138 4139 4140 4141 4142 4143 4144 4044 4045 4046 4047 4048 4049 4050 4051 4053 4054 4055 4056 4060 4061 4062 4063 4064 4065 4067 4076 4077 4078 4079 4080 4081 4082 4083 4084 4085 4086 4087 4091 4092 4093 4094 4095 4096 4097 4098 4099 4100 4101 4102 4104 5047 5235 5239 5012 5013 5014 5015 5016 5017 5018 5019 5020 5021 5022 5023 5024 5079 5080 5081 5082 5083 5084 5085 5086 5087 5088 5089 5090 5240 5243 5245 5253 5254 5256 5257 5258 5259 5260 5261 5262 5263 5264 5265 5269 5270 5271 5272 5273 5274 5275 5276 5277 5278 5279 5188 5192 5193 5194 5195 5196 5197 5201 5202 5203 5204 5205 5206 5207 5208 5209 5210 5211 5212 5213 5214 5215 5216 5220 5222 5224 5225 5226 5227 5228 5229 5231 5232 5233 5234 5132 5133 5134 5135 5136 5137 5138 5141 5142 5145 5147 5148 5149 5150 5154 5155 5156 5157 5158 5159 5166 5168 5169 5171 5172 5173 5175 5176 5177 5178 5179 5180 5181 5184 5185 5186 5284 5288 5289 5290 5291 5292 5294 5295 5296 5297 5298 5304 5305 5306 5308 5309 5310 5311 5312 5313 5314 5066 5095 5096 5097 5098 5099 5100 5101 5102 5103 5104 5105 5106 5110 5111 5113 5114 5115 5116 5117 5118 5119 5120 5121 5122 5123 5318 5322 5323 5324 5325 5326 5327 5328 5329 5330 5331 5332 5333 5334 5335 5336 5337 6041 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6056 6057 6061 6062 6063 6064 6065 6066 6067 6068 6070 6071 6072 6073 6116 6117 6118 6119 6120 6121 6122 6123 6124 6125 6126 6130 6131 6132 6133 6134 6080 6081 6082 6083 6084 6085 6086 6087 6089 6090 6091 6092 6093 6097 6099 6100 6101 6102 6103 6104 6105 6106 6107 6108)

#EVENTIDDIFFLIST=("1" "2")
#EVENTIDDIFFLIST=("0")
#EVENTIDDIFF="-1"
#for EVENTIDDIFF in "${EVENTIDDIFFLIST[@]}"
#do

DUTLIST=(1 2 3 4 5 6)
#TEMPLATELIST=('alibava-converter' 'alibava-reco' 'alibava-applyxtalk' 'telescope-converter' 'telescope-clustering' 'telescope-filter' 'merger' 'hitmaker-local')
TEMPLATELIST=('merger' 'hitmaker-local')

for Template in "${TEMPLATELIST[@]}"
do

for DUT in "${DUTLIST[@]}"
do

if [ $DUT = 1 ]; then
RUNLIST=(1053 1057 1061 1062 1063 1064 1065 1066 1067 1068 1069 1070 1076 1077 1078 1079 1082 1083 1084 1085 1086 1087 1088 1089 1090 1091 1092 1223 1224 1225 1226 1227 1228 1229 1230 1231 1232 1233 1234 1235 1239 1240 1241 1242 1243 1244 1245 1246 1247 1248 1249 1250 1186 1187 1188 1189 1190 1191 1192 1194 1195 1196 1197 1198 1199 1205 1206 1207 1208 1209 1210 1211 1212 1213 1214 1215 1216 1148 1149 1150 1151 1152 1153 1154 1155 1157 1158 1159 1160 1161 1165 1166 1167 1168 1169 1170 1171 1172 1173 1174 1175 1176 1180 1103 1105 1106 1107 1108 1117 1118 1119 1120 1121 1122 1123 1124 1128 1129 1130 1131 1132 1133 1134 1135 1136 1137 1138 1139 1261 1262)
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 2 ]; then
RUNLIST=(2033 2034 2035 2036 2037 2039 2040 2041 2042 2043 2044 2045 2046 2052 2053 2054 2055 2056 2058 2059 2060 2061 2062 2064 2067 2071 2297 2334 2335 2336 2337 2338 2343 2344 2345 2192 2193 2194 2195 2196 2197 2203 2204 2206 2207 2208 2209 2210 2214 2215 2216 2217 2218 2219 2220 2221 2222 2223 2224 2225 2154 2155 2156 2157 2158 2159 2160 2161 2163 2164 2165 2166 2167 2168 2169 2173 2174 2175 2176 2177 2178 2179 2180 2181 2182 2319 2320 2321 2322 2324 2328 2329 2330 2331 2332 2333 2118 2120 2121 2122 2123 2124 2125 2126 2127 2128 2129 2130 2131 2135 2136 2137 2138 2140 2141 2142 2143 2144 2145 2146 2147 2236 2237 2238 2239 2240 2241 2243 2244 2245 2246 2247 2248 2249 2250 2255 2256 2258 2259 2260 2261 2262 2263 2264 2265 2266 2268 2081 2085 2086 2087 2088 2089 2090 2091 2092 2093 2094 2095 2096 2100 2101 2102 2103 2104 2105 2106 2107 2108 2109 2110 2111 2272 2273 2274 2275 2276 2281 2282 2283 2284 2285 2286)
BONDS='$0:24-126$ $1:24-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 3 ]; then
RUNLIST=(3044 3045 3046 3047 3048 3049 3050 3051 3052 3053 3054 3055 3056 3061 3062 3063 3064 3065 3066 3067 3068 3069 3070 3071 3072 3195 3196 3197 3198 3199 3200 3201 3214 3215 3216 3217 3218 3219 3220 3221 3222 3223 3224 3225 3226 3227 3231 3232 3233 3234 3157 3158 3159 3160 3161 3162 3163 3164 3165 3171 3172 3173 3174 3175 3176 3178 3179 3180 3181 3182 3183 3184 3185 3186 3187 3120 3121 3122 3123 3124 3125 3126 3127 3128 3129 3130 3131 3135 3136 3137 3138 3139 3140 3143 3144 3145 3146 3147 3148 3149 3080 3081 3082 3083 3084 3085 3086 3087 3088 3090 3091 3092 3093 3098 3099 3100 3101 3102 3103 3104 3106 3107 3108 3110 3111)
BONDS='$0:20-22$ $0:24-29$ $0:31-50$ $0:56-122$ $1:24-112$ $1:114-126$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 4 ]; then
RUNLIST=(4003 4004 4005 4006 4007 4008 4009 4010 4011 4012 4013 4014 4015 4021 4022 4023 4024 4025 4026 4027 4028 4029 4030 4031 4185 4186 4187 4188 4189 4190 4191 4192 4193 4194 4195 4196 4197 4201 4202 4203 4204 4205 4206 4207 4208 4209 4210 4211 4212 4151 4152 4153 4154 4155 4156 4157 4158 4159 4160 4161 4162 4163 4167 4168 4169 4170 4171 4172 4173 4174 4175 4176 4177 4113 4114 4115 4116 4117 4118 4119 4120 4125 4126 4127 4128 4129 4133 4134 4135 4136 4137 4138 4139 4140 4141 4142 4143 4144 4044 4045 4046 4047 4048 4049 4050 4051 4053 4054 4055 4056 4060 4061 4062 4063 4064 4065 4067 4076 4077 4078 4079 4080 4081 4082 4083 4084 4085 4086 4087 4091 4092 4093 4094 4095 4096 4097 4098 4099 4100 4101 4102 4104)
BONDS='$0:19-33$ $0:35-121$ $1:23-35$ $1:39-76$ $1:78-125$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi


if [ $DUT = 5 ]; then
RUNLIST=(5047 5235 5239 5012 5013 5014 5015 5016 5017 5018 5019 5020 5021 5022 5023 5024 5079 5080 5081 5082 5083 5084 5085 5086 5087 5088 5089 5090 5240 5243 5245 5253 5254 5256 5257 5258 5259 5260 5261 5262 5263 5264 5265 5269 5270 5271 5272 5273 5274 5275 5276 5277 5278 5279 5188 5192 5193 5194 5195 5196 5197 5201 5202 5203 5204 5205 5206 5207 5208 5209 5210 5211 5212 5213 5214 5215 5216 5220 5222 5224 5225 5226 5227 5228 5229 5231 5232 5233 5234 5132 5133 5134 5135 5136 5137 5138 5141 5142 5145 5147 5148 5149 5150 5154 5155 5156 5157 5158 5159 5166 5168 5169 5171 5172 5173 5175 5176 5177 5178 5179 5180 5181 5184 5185 5186 5284 5288 5289 5290 5291 5292 5294 5295 5296 5297 5298 5304 5305 5306 5308 5309 5310 5311 5312 5313 5314 5066 5095 5096 5097 5098 5099 5100 5101 5102 5103 5104 5105 5106 5110 5111 5113 5114 5115 5116 5117 5118 5119 5120 5121 5122 5123 5318 5322 5323 5324 5325 5326 5327 5328 5329 5330 5331 5332 5333 5334 5335 5336 5337)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

if [ $DUT = 6 ]; then
RUNLIST=(6041 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6056 6057 6061 6062 6063 6064 6065 6066 6067 6068 6070 6071 6072 6073 6116 6117 6118 6119 6120 6121 6122 6123 6124 6125 6126 6130 6131 6132 6133 6134 6080 6081 6082 6083 6084 6085 6086 6087 6089 6090 6091 6092 6093 6097 6099 6100 6101 6102 6103 6104 6105 6106 6107 6108)
BONDS='$0:20-122$ $1:24-127$'
TIMECUTMIN=5.0
TIMECUTMAX=15.0
fi

CONFIG=config/dut${DUT}_lam_config.cfg
cp config/noCuts_config.cfg $CONFIG
sed -i "/Bonds/ c\Bonds=$BONDS" $CONFIG 
sed -i "/TimeCutMin/ c\TimeCutMin=$TIMECUTMIN" $CONFIG 
sed -i "/TimeCutMax/ c\TimeCutMax=$TIMECUTMAX" $CONFIG 
#sed -i "/EventIDDifference/ c\EventIDDifference=0" $CONFIG
#sed -i "/EventIDDifference/ c\EventIDDifference=${EVENTIDDIFF}" $CONFIG
sed -i "/EventIDDifference/ c\EventIDDifference=@EventIDDiff@" $CONFIG 

#echo "Running EventIdDiff=${EVENTIDDIFF}, DUT=${DUT}"


CSV=runlistfiles/lam_runlist_all.csv
CONFIG=config/dut${DUT}_lam_config.cfg
echo Config file: $CONFIG

jobsub -c $CONFIG -csv $CSV --subdir --naf qsubparameters.cfg $Template $RUNLIST

done # DUTLIST

source myscripts/continue_when_all_runs_finished.sh
#source myscripts/ask_me_to_continue.sh

done # TEMPLATELIST
root -q -b 'lam_plots_cluster.C(1)' 
root -q -b 'lam_plots_cluster.C(2)'
root -q -b 'lam_plots_cluster.C(3)'
root -q -b 'lam_plots_cluster.C(4)'
root -q -b 'lam_plots_cluster.C(5)'
root -q -b 'lam_plots_cluster.C(6)'

root -q -b 'lam_plots_hit.C(1)' 
root -q -b 'lam_plots_hit.C(2)'
root -q -b 'lam_plots_hit.C(3)'
root -q -b 'lam_plots_hit.C(4)'
root -q -b 'lam_plots_hit.C(5)'
root -q -b 'lam_plots_hit.C(6)'

########## produce hit plots
#for DUT in "${DUTLIST[@]}"
#do

#mv pdfs/lam_plots_hit_dut${DUT}.pdf pdfs/lam_plots_hit_dut${DUT}_idDiff${EVENTIDDIFF}.pdf
#done # DUTLIST

############# move result files
#for runnum in "${ALLRUNLIST[@]}"
#do

#OUTPUTPATH="/nfs/dust/atlas/user/yeda/EUTelescopeOutput/output"
#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local.slcio ${OUTPUTPATH}/lcio/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.slcio
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-merger_idDiff${EVENTIDDIFF}.root
#mv ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local.root ${OUTPUTPATH}/histogram/run${(l:6::0:)runnum}-hitmaker-local_idDiff${EVENTIDDIFF}.root


#done # ALLRUNLIST

#done # EVENTIDDIFFLIST
