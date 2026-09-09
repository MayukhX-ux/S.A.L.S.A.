<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>S.A.L.S.A | Secure Acoustic Ledger for Subsea Autonomy</title>
    <style>
:root{
    --bg:#050914;
    --bg-soft:#08111f;
    --surface:rgba(13,24,40,.82);
    --surface-2:rgba(17,30,49,.92);
    --border:rgba(148,163,184,.16);
    --border-strong:rgba(56,189,248,.45);
    --cyan:#38d9ff;
    --blue:#4f7cff;
    --green:#34f5a4;
    --orange:#ffae57;
    --text:#edf6ff;
    --muted:#9fb0c4;
    --muted-2:#72839a;
    --shadow:0 24px 70px rgba(0,0,0,.30);
    --radius:20px;
}

*{margin:0;padding:0;box-sizing:border-box}
html{scroll-behavior:smooth}
body{
    background:
        radial-gradient(circle at 15% 8%,rgba(56,217,255,.09),transparent 28rem),
        radial-gradient(circle at 85% 20%,rgba(79,124,255,.09),transparent 30rem),
        linear-gradient(180deg,#050914 0%,#07101d 45%,#050914 100%);
    color:var(--text);
    font-family:Inter,ui-sans-serif,system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;
    line-height:1.7;
    overflow-x:hidden;
}
body:before{
    content:"";
    position:fixed;inset:0;pointer-events:none;z-index:-1;
    background-image:
      linear-gradient(rgba(255,255,255,.018) 1px,transparent 1px),
      linear-gradient(90deg,rgba(255,255,255,.018) 1px,transparent 1px);
    background-size:48px 48px;
    mask-image:linear-gradient(to bottom,black,transparent 80%);
}
::selection{background:rgba(56,217,255,.25);color:#fff}

.hero{
    min-height:86vh;
    display:flex;flex-direction:column;justify-content:center;align-items:center;
    text-align:center;padding:6rem 2rem 5rem;position:relative;overflow:hidden;
    background:
      radial-gradient(circle at center,rgba(24,51,82,.52) 0%,rgba(5,9,20,.08) 48%,transparent 75%);
    border-bottom:1px solid var(--border);
}
.hero:after{
    content:"";
    position:absolute;inset:auto 0 0;height:1px;
    background:linear-gradient(90deg,transparent,var(--cyan),var(--blue),transparent);
    opacity:.7;
}
.hero:before{
    content:"";
    position:absolute;width:720px;height:720px;border-radius:50%;
    background:radial-gradient(circle,rgba(56,217,255,.11),rgba(56,217,255,.025) 35%,transparent 68%);
    animation:pulse 5s ease-in-out infinite;
}
@keyframes pulse{0%,100%{transform:scale(.94);opacity:.55}50%{transform:scale(1.05);opacity:1}}

.hero h1{
    font-size:clamp(3.5rem,9vw,7rem);font-weight:900;letter-spacing:.28em;
    line-height:1;margin-left:.28em;
    background:linear-gradient(120deg,#dffaff 0%,var(--cyan) 30%,var(--blue) 68%,var(--green) 100%);
    -webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;
    filter:drop-shadow(0 0 30px rgba(56,217,255,.18));z-index:1;
    animation:fadeInDown .9s ease;
}
.hero .subtitle{
    font-size:clamp(1rem,2vw,1.3rem);color:#c3d0df;margin-top:1.35rem;
    letter-spacing:.03em;z-index:1;animation:fadeInUp .9s ease .15s both;
}
.hero .theme{
    display:inline-flex;align-items:center;gap:.55rem;margin-top:1.6rem;
    padding:.55rem 1.15rem;border:1px solid rgba(56,217,255,.32);
    border-radius:999px;background:rgba(56,217,255,.055);backdrop-filter:blur(10px);
    color:var(--cyan);font-size:.78rem;font-weight:700;letter-spacing:.12em;
    text-transform:uppercase;z-index:1;animation:fadeIn .9s ease .35s both;
    box-shadow:0 0 0 4px rgba(56,217,255,.025);
}
.hero .theme:before{content:"";width:7px;height:7px;border-radius:50%;background:var(--green);box-shadow:0 0 12px var(--green)}
@keyframes fadeInDown{from{opacity:0;transform:translateY(-22px)}to{opacity:1;transform:none}}
@keyframes fadeInUp{from{opacity:0;transform:translateY(22px)}to{opacity:1;transform:none}}
@keyframes fadeIn{from{opacity:0}to{opacity:1}}

.container{max-width:1240px;margin:0 auto;padding:5rem 2rem}
.section{
    margin-bottom:6rem;opacity:0;transform:translateY(26px);
    transition:opacity .7s ease,transform .7s ease;
    scroll-margin-top:2rem;
}
.section.visible{opacity:1;transform:none}
.section h2{
    position:relative;font-size:clamp(1.9rem,4vw,2.65rem);font-weight:800;
    letter-spacing:-.025em;margin-bottom:2rem;padding-bottom:1rem;
    color:#f2f8ff;background:none;-webkit-text-fill-color:initial;
}
.section h2:after{
    content:"";position:absolute;left:0;bottom:0;width:78px;height:3px;border-radius:99px;
    background:linear-gradient(90deg,var(--cyan),var(--blue));
    box-shadow:0 0 16px rgba(56,217,255,.28);
}
.section h2:before{
    content:"//";color:var(--cyan);font-family:"JetBrains Mono",monospace;
    font-size:.55em;margin-right:.65rem;vertical-align:.22em;opacity:.8;
}
.section h3{font-size:1.5rem;color:var(--cyan);margin:1.5rem 0 1rem}

.card-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(285px,1fr));gap:1.25rem;margin:2rem 0}
.card{
    background:linear-gradient(145deg,rgba(17,30,49,.92),rgba(9,19,33,.82));
    border:1px solid var(--border);border-radius:var(--radius);padding:1.65rem;
    transition:transform .28s ease,border-color .28s ease,box-shadow .28s ease;
    position:relative;overflow:hidden;box-shadow:0 12px 35px rgba(0,0,0,.12);
}
.card:before{
    content:"";position:absolute;inset:0 0 auto;height:2px;
    background:linear-gradient(90deg,var(--cyan),var(--blue),var(--green));
    transform:scaleX(0);transform-origin:left;transition:transform .35s ease;
}
.card:hover{transform:translateY(-6px);border-color:var(--border-strong);box-shadow:var(--shadow)}
.card:hover:before{transform:scaleX(1)}
.card h4{color:#e9faff;font-size:1.05rem;margin-bottom:.7rem}
.card h4:before{content:"◆";font-size:.55rem;color:var(--cyan);margin-right:.6rem;vertical-align:.15em}
.card p{color:var(--muted);font-size:.93rem}

.image-container{
    background:linear-gradient(145deg,rgba(15,28,47,.94),rgba(7,15,27,.94));
    border:1px solid var(--border);border-radius:var(--radius);padding:1rem;
    margin:1.5rem 0;transition:transform .3s ease,border-color .3s ease,box-shadow .3s ease;
    box-shadow:0 14px 40px rgba(0,0,0,.18);
}
.image-container:hover{transform:translateY(-3px);border-color:var(--border-strong);box-shadow:var(--shadow)}
.image-container img{
    width:100%;display:block;border-radius:13px;margin-bottom:1rem;
    background:#030711;object-fit:contain;
}
.image-container .caption{
    color:var(--muted-2);text-align:center;font-size:.84rem;
    letter-spacing:.035em;padding:.15rem .5rem .25rem;
}

.tech-badge{
    display:inline-flex;align-items:center;padding:.48rem .9rem;
    background:rgba(56,217,255,.055);border:1px solid rgba(56,217,255,.18);
    border-radius:999px;color:#bdefff;font-size:.78rem;font-weight:650;
    margin:.25rem;transition:all .25s ease;backdrop-filter:blur(8px);
}
.tech-badge:before{content:"";width:5px;height:5px;border-radius:50%;background:var(--cyan);margin-right:.48rem}
.tech-badge:hover{background:rgba(56,217,255,.11);border-color:rgba(56,217,255,.45);transform:translateY(-2px)}

.flowchart{
    background:#030811;border:1px solid var(--border);border-radius:var(--radius);
    padding:1.5rem;position:relative;overflow:auto;
    box-shadow:inset 0 1px 0 rgba(255,255,255,.025),0 15px 45px rgba(0,0,0,.22);
}
.flowchart:before{
    content:"SYSTEM LOG";position:absolute;top:12px;right:16px;
    color:var(--muted-2);font:600 .62rem "JetBrains Mono",monospace;letter-spacing:.15em;
}
.flowchart pre{
    color:#9eeeff;font-size:.79rem;line-height:1.35;
    font-family:"JetBrains Mono","SFMono-Regular",Consolas,monospace;
    padding-top:1rem;
}
.flowchart .arrow{color:var(--orange)}
.flowchart .decision{color:var(--green)}

.team-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(190px,1fr));gap:1rem}
.team-card{
    background:linear-gradient(145deg,rgba(17,30,49,.94),rgba(8,17,30,.9));
    border:1px solid var(--border);border-radius:18px;padding:1.35rem;text-align:center;
    transition:transform .28s ease,border-color .28s ease,box-shadow .28s ease;
}
.team-card:hover{border-color:rgba(52,245,164,.42);transform:translateY(-5px);box-shadow:0 18px 45px rgba(0,0,0,.24)}
.team-card .avatar{
    width:64px;height:64px;border-radius:18px;
    background:linear-gradient(135deg,rgba(56,217,255,.95),rgba(79,124,255,.95));
    margin:0 auto 1rem;display:flex;justify-content:center;align-items:center;
    font-size:1.15rem;color:white;font-weight:850;box-shadow:0 8px 25px rgba(56,217,255,.12);
}
.team-card h5{color:var(--text);font-size:.94rem;line-height:1.4}
.team-card .role{color:var(--cyan);font-size:.78rem;font-weight:700;margin-top:.35rem}
.team-card .branch{color:var(--muted-2);font-size:.74rem;margin-top:.15rem}

.link-placeholder{
    display:inline-flex;align-items:center;padding:.9rem 1.2rem;
    background:linear-gradient(135deg,rgba(56,217,255,.09),rgba(79,124,255,.08));
    border:1px solid rgba(56,217,255,.32);border-radius:14px;color:#c9f7ff;
    text-decoration:none;font-size:.95rem;font-weight:700;transition:all .25s ease;margin:1rem 0;
    box-shadow:0 10px 30px rgba(0,0,0,.15);
}
.link-placeholder:hover{background:rgba(56,217,255,.14);border-color:var(--cyan);transform:translateY(-2px)}

.stats-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(145px,1fr));gap:1rem;margin:2.25rem 0}
.stat-card{
    background:linear-gradient(145deg,rgba(17,30,49,.9),rgba(8,17,30,.86));
    border:1px solid var(--border);border-radius:18px;padding:1.35rem;text-align:center;
    position:relative;overflow:hidden;
}
.stat-card:after{content:"";position:absolute;left:15%;right:15%;bottom:0;height:1px;background:linear-gradient(90deg,transparent,var(--cyan),transparent);opacity:.35}
.stat-card .number{font-size:1.8rem;font-weight:850;letter-spacing:-.03em;color:#e8fbff}
.stat-card .label{color:var(--muted-2);font-size:.76rem;margin-top:.3rem;line-height:1.35}

table{
    width:100%;border-collapse:separate;border-spacing:0;margin:2rem 0;
    background:rgba(10,20,34,.82);border:1px solid var(--border);border-radius:18px;overflow:hidden;
    box-shadow:0 14px 40px rgba(0,0,0,.15);
}
th,td{padding:1rem 1.1rem;text-align:left;border-bottom:1px solid var(--border)}
th{background:rgba(56,217,255,.065);color:#bff5ff;font-weight:750;font-size:.82rem;letter-spacing:.03em}
td{color:var(--muted);font-size:.9rem}
tr:last-child td{border-bottom:0}
tbody tr{transition:background .2s ease}
tbody tr:hover td{background:rgba(56,217,255,.035);color:#cbd8e7}

.footer{
    text-align:center;padding:3.5rem 2rem;border-top:1px solid var(--border);
    color:var(--muted);background:rgba(3,8,17,.72);
}
.footer p:first-child{color:#dffaff;font-weight:750}
.footer p:last-child{color:var(--muted-2)}

@media(max-width:768px){
    .hero{min-height:72vh;padding:4.5rem 1.25rem}
    .hero h1{font-size:3.2rem;letter-spacing:.16em;margin-left:.16em}
    .hero .theme{font-size:.68rem;text-align:center}
    .container{padding:3.5rem 1rem}
    .section{margin-bottom:4.5rem}
    .section h2{font-size:1.85rem}
    .card-grid{grid-template-columns:1fr}
    .flowchart{padding:1rem}
    .flowchart pre{font-size:.66rem}
    th,td{padding:.75rem;font-size:.78rem}
}
@media(prefers-reduced-motion:reduce){
    html{scroll-behavior:auto}
    *,*:before,*:after{animation:none!important;transition:none!important}
}
</style>
</head>
<body>

<nav aria-label="Primary navigation" style="
position:sticky;top:0;z-index:50;display:flex;justify-content:center;gap:.35rem;
padding:.65rem 1rem;background:rgba(5,9,20,.78);backdrop-filter:blur(18px);
border-bottom:1px solid rgba(148,163,184,.12);overflow-x:auto;">
  <a href="#overview" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Overview</a>
  <a href="#prototype" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Prototype</a>
  <a href="#circuit" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Circuit</a>
  <a href="#architecture" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Architecture</a>
  <a href="#algorithm" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Algorithm</a>
  <a href="#waveforms" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Waveforms</a>
  <a href="#features" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Features</a>
  <a href="#tech" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Tech Stack</a>
  <a href="#team" style="color:#9fb0c4;text-decoration:none;padding:.4rem .65rem;font-size:.74rem;font-weight:700;white-space:nowrap;">Team</a>
</nav>


    <!-- HERO SECTION -->
    <div class="hero">
        <h1>S.A.L.S.A</h1>
        <p class="subtitle">Secure Acoustic Ledger for Subsea Autonomy</p>
        <span class="theme">Robotics & Drones | Smart India Hackathon 2026</span>
    </div>

    <div class="container">

        <!-- PROJECT OVERVIEW -->
        <div class="section" id="overview">
            <h2>Project Overview</h2>
            <p style="color: var(--muted); font-size: 1.1rem; margin-bottom: 2rem;">
                S.A.L.S.A is an adaptive sonar transmitter payload designed for Autonomous Underwater Vehicles (AUVs). 
                It dynamically adjusts acoustic waveform parameters based on real-time environmental conditions, 
                ensuring optimal performance across diverse underwater scenarios. The system uses a custom R-2R 
                ladder DAC for 12-bit resolution, multiple waveform generation, digital windowing, and a complete 
                analog filter chain for clean signal output.
            </p>
            <div class="stats-grid">
                <div class="stat-card">
                    <div class="number">100-500</div>
                    <div class="label">Frequency Range (kHz)</div>
                </div>
                <div class="stat-card">
                    <div class="number">12-bit</div>
                    <div class="label">DAC Resolution</div>
                </div>
                <div class="stat-card">
                    <div class="number">35-80</div>
                    <div class="label">Power Draw (mA)</div>
                </div>
                <div class="stat-card">
                    <div class="number">5</div>
                    <div class="label">Waveform Types</div>
                </div>
                <div class="stat-card">
                    <div class="number">3</div>
                    <div class="label">Digital Windows</div>
                </div>
            </div>
        </div>

        <!-- PROTOTYPE GALLERY -->
        <div class="section" id="prototype">
            <h2>Prototype Gallery</h2>
            <p style="color: var(--muted); margin-bottom: 2rem;">Evolution of the S.A.L.S.A prototype from initial breadboard to final enclosure.</p>
            
            <div class="image-container">
                <img src="assets/Initial Prototype.jpeg" alt="Initial Breadboard Prototype" onerror="this.style.display='none'">
                <div class="caption">Initial Breadboard Prototype — Core ESP32 with R-2R ladder testing</div>
            </div>

            <div class="image-container">
                <img src="assets/Prototype_v2.jpeg" alt="Second Stage Prototype" onerror="this.style.display='none'">
                <div class="caption">Prototype v2 — Integrated filter chain and OLED display</div>
            </div>

            <div class="image-container">
                <img src="assets/3D enclosure fabricated.jpeg" alt="3D Printed Enclosure" onerror="this.style.display='none'">
                <div class="caption">3D Printed Enclosure — Field-deployable AUV payload pod</div>
            </div>
        </div>

        <!-- CIRCUIT DIAGRAM -->
        <div class="section" id="circuit">
            <h2>Circuit Diagram</h2>
            <p style="color: var(--muted); margin-bottom: 2rem;">Complete schematic for the S.A.L.S.A payload showing all interconnections.</p>
            
            <div class="image-container">
                <img src="assets/Circuit Diagram.png" alt="Complete Circuit Diagram" onerror="this.style.display='none'">
                <div class="caption">Complete Circuit Schematic — ESP32, R-2R ladder, filter chain, and peripherals</div>
            </div>
        </div>

        <!-- SYSTEM ARCHITECTURE FLOWCHART -->
        <div class="section" id="architecture">
            <h2>System Architecture</h2>
            <p style="color: var(--muted); margin-bottom: 2rem;">Complete workflow from environmental sensing to analog output.</p>

            <div class="flowchart">
                <pre>
┌─────────────────────────────────────────────────────────────────────┐
│                        S.A.L.S.A SYSTEM FLOW                        │
└─────────────────────────────────────────────────────────────────────┘

┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│  Turbidity    │     │    Depth      │     │  Temperature  │
│ Potentiometer │     │ Potentiometer │     │   (Fixed)     │
│   (GPIO36)    │     │   (GPIO39)    │     │    25°C       │
└───────┬───────┘     └───────┬───────┘     └───────┬───────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              ▼
                    ┌──────────────────┐
                    │   12-bit ADC     │
                    │  30-Sample Avg   │
                    └──────────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │  ADAPTIVE LOGIC  │◄──────┐
                    │  ESP32 Firmware  │       │
                    └──────────────────┘       │
                              │                │
                              ▼                │
                    ┌──────────────────┐       │
                    │ WAVEFORM ENGINE  │       │
                    │  5 Wave Types    │       │
                    └──────────────────┘       │
                              │                │
                              ▼                │
                    ┌──────────────────┐       │
                    │ DIGITAL WINDOW   │       │
                    │ Hamming/Hann/    │       │
                    │ Blackman         │       │
                    └──────────────────┘       │
                              │                │
                              ▼                │
                    ┌──────────────────┐       │
                    │ R-2R LADDER DAC  │       │
                    │  12-bit Output   │       │
                    └──────────────────┘       │
                              │                │
                              ▼                │
                    ┌──────────────────┐       │
                    │  ANALOG FILTER   │       │
                    │ 10µF + RC + TL072│       │
                    └──────────────────┘       │
                              │                │
                              ▼                │
                    ┌──────────────────┐       │
                    │   BNC OUTPUT     │       │
                    │  Oscilloscope    │       │
                    └──────────────────┘       │
                              │                │
                              └────────────────┘
                              (Real-time loop)
                </pre>
            </div>
        </div>

        <!-- ADAPTIVE ALGORITHM FLOWCHART -->
        <div class="section" id="algorithm">
            <h2>Adaptive Algorithm</h2>
            <p style="color: var(--muted); margin-bottom: 2rem;">Decision logic for environment-based waveform selection.</p>

            <div class="flowchart">
                <pre>
┌─────────────────────────────────────────────────────────────┐
│                  ADAPTIVE DECISION ALGORITHM                │
└─────────────────────────────────────────────────────────────┘

                    ┌─────────────┐
                    │ START LOOP  │
                    └──────┬──────┘
                           ▼
                    ┌─────────────┐
                    │ READ SENSORS│
                    │ Turbidity % │
                    │ Depth %     │
                    └──────┬──────┘
                           ▼
                 ┌─────────────────┐
                 │ Turbidity < 20% │
                 └────┬───────┬────┘
                    YES       NO
                     │         │
                     ▼         ▼
              ┌────────────┐ ┌─────────────────┐
              │ 500 kHz    │ │ Turbidity < 40% │
              │ LFM Up     │ └────┬───────┬────┘
              │ 5 ms       │    YES       NO
              │ 49% Power  │     │         │
              └────────────┘     ▼         ▼
                          ┌────────────┐ ┌─────────────────┐
                          │ 400 kHz    │ │ Turbidity < 60% │
                          │ LFM Down   │ └────┬───────┬────┘
                          │ 8 ms       │    YES       NO
                          │ 61% Power  │     │         │
                          └────────────┘     ▼         ▼
                                      ┌────────────┐ ┌─────────────────┐
                                      │ 300 kHz    │ │ Turbidity < 80% │
                                      │ CW Pulse   │ └────┬───────┬────┘
                                      │ 12 ms      │    YES       NO
                                      │ 73% Power  │     │         │
                                      └────────────┘     ▼         ▼
                                                   ┌────────────┐ ┌────────────┐
                                                   │ 200 kHz    │ │ 100 kHz    │
                                                   │ Phase Coded│ │ Geometric  │
                                                   │ 15 ms      │ │ 20 ms      │
                                                   │ 85% Power  │ │ 100% Power │
                                                   └────────────┘ └────────────┘
                                                            │
                                                            ▼
                                                  ┌──────────────────┐
                                                  │ DEPTH ADJUSTMENT │
                                                  │ >70%: Power+500 │
                                                  │ <30%: Power-300 │
                                                  └──────────────────┘
                                                            │
                                                            ▼
                                                  ┌──────────────────┐
                                                  │ GENERATE WAVEFORM│
                                                  │ APPLY WINDOW     │
                                                  │ OUTPUT TO DAC    │
                                                  └──────────────────┘
                                                            │
                                                            ▼
                                                    ┌─────────────┐
                                                    │  LOOP AGAIN │
                                                    └─────────────┘
                </pre>
            </div>
        </div>

        <!-- WAVEFORM TYPES TABLE -->
        <div class="section" id="waveforms">
            <h2>Waveform Types</h2>
            <table>
                <thead>
                    <tr>
                        <th>Waveform</th>
                        <th>Frequency Range</th>
                        <th>Application</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>LFM Up Chirp</td>
                        <td>500 kHz (Clear Water)</td>
                        <td>High resolution imaging</td>
                    </tr>
                    <tr>
                        <td>LFM Down Chirp</td>
                        <td>400 kHz (Moderate)</td>
                        <td>Balanced performance</td>
                    </tr>
                    <tr>
                        <td>CW Pulse</td>
                        <td>300 kHz (Muddy)</td>
                        <td>Continuous transmission</td>
                    </tr>
                    <tr>
                        <td>Phase Coded (Barker-7)</td>
                        <td>200 kHz (Heavy Mud)</td>
                        <td>Penetration mode</td>
                    </tr>
                    <tr>
                        <td>Geometric Sweep</td>
                        <td>100 kHz (Extreme)</td>
                        <td>Maximum penetration</td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- KEY FEATURES -->
        <div class="section" id="features">
            <h2>Key Features</h2>
            <div class="card-grid">
                <div class="card">
                    <h4>Adaptive Frequency Switching</h4>
                    <p>Real-time automatic selection between 100 kHz and 500 kHz based on turbidity and depth inputs from potentiometers.</p>
                </div>
                <div class="card">
                    <h4>Custom R-2R Ladder DAC</h4>
                    <p>12-bit digital-to-analog conversion using 10k and 20k (2×10k series) resistors with perfect 2:1 ratio for accurate output.</p>
                </div>
                <div class="card">
                    <h4>Multi-Waveform Generation</h4>
                    <p>CW, LFM Up/Down, Phase-Coded (Barker-7), and Geometric Sweep waveforms generated on-the-fly.</p>
                </div>
                <div class="card">
                    <h4>Digital Windowing</h4>
                    <p>Hamming (default), Hann, and Blackman windows for sidelobe suppression and smooth pulse transitions.</p>
                </div>
                <div class="card">
                    <h4>Analog Filter Chain</h4>
                    <p>10µF coupling capacitor, 10kΩ+100pF RC low-pass filter, and TL072 op-amp buffer for clean signal output.</p>
                </div>
                <div class="card">
                    <h4>Low Power Design</h4>
                    <p>Dynamic CPU frequency scaling (80-240 MHz) with adaptive current draw of 35-80 mA for extended battery life.</p>
                </div>
            </div>
        </div>

        <!-- TECHNOLOGY STACK -->
        <div class="section" id="tech">
            <h2>Technology Stack</h2>
            <div style="margin: 2rem 0;">
                <span class="tech-badge">ESP32 Microcontroller</span>
                <span class="tech-badge">R-2R Ladder DAC</span>
                <span class="tech-badge">TL072 Op-Amp</span>
                <span class="tech-badge">SSD1306 OLED</span>
                <span class="tech-badge">Arduino IDE</span>
                <span class="tech-badge">C/C++</span>
                <span class="tech-badge">I2C Protocol</span>
                <span class="tech-badge">Parallel GPIO</span>
                <span class="tech-badge">DSP Algorithms</span>
                <span class="tech-badge">LFM Chirp</span>
                <span class="tech-badge">Barker-7 Phase Code</span>
                <span class="tech-badge">Hamming Window</span>
                <span class="tech-badge">ESP32 Power Management</span>
            </div>
        </div>

        <!-- DEPLOYED WEBSITE -->
        <div class="section" id="deployment">
            <h2>Live Demo</h2>
            <p style="color: var(--muted); margin-bottom: 2rem;">Interactive visualization and deployed website for the S.A.L.S.A project.</p>
            <a href="#" class="link-placeholder" id="deployLink">🔗 Deployed Website Link</a>
        </div>

        <!-- TEAM -->
        <div class="section" id="team">
            <h2>Team Details</h2>
            <div class="team-grid">
                <div class="team-card">
                    <div class="avatar">T1</div>
                    <h5>Team Lead & Firmware Architect</h5>
                    <div class="role">CSE</div>
                    <div class="branch">System Design & Integration</div>
                </div>
                <div class="team-card">
                    <div class="avatar">T2</div>
                    <h5>Embedded Systems Engineer</h5>
                    <div class="role">ET&T</div>
                    <div class="branch">Microcontroller Programming</div>
                </div>
                <div class="team-card">
                    <div class="avatar">T3</div>
                    <h5>Signal Processing Specialist</h5>
                    <div class="role">ET&T</div>
                    <div class="branch">Filter Design & DSP</div>
                </div>
                <div class="team-card">
                    <div class="avatar">T4</div>
                    <h5>Software & Simulation Engineer</h5>
                    <div class="role">CSE</div>
                    <div class="branch">Algorithm Development</div>
                </div>
                <div class="team-card">
                    <div class="avatar">T5</div>
                    <h5>UI/UX & Data Analyst</h5>
                    <div class="role">CSE</div>
                    <div class="branch">OLED Interface & Visualization</div>
                </div>
                <div class="team-card">
                    <div class="avatar">T6</div>
                    <h5>Mechanical Design Engineer</h5>
                    <div class="role">Mech</div>
                    <div class="branch">Enclosure & 3D Design</div>
                </div>
            </div>
        </div>

    </div>

    <div class="footer">
        <p>S.A.L.S.A | Secure Acoustic Ledger for Subsea Autonomy</p>
        <p style="margin-top: 0.5rem;">Smart India Hackathon 2026 | Robotics & Drones Theme</p>
        <p style="margin-top: 0.5rem; font-size: 0.85rem;">&copy; 2026 S.A.L.S.A Project Team. All Rights Reserved.</p>
    </div>

    <script>
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('visible');
                }
            });
        }, { threshold: 0.1 });

        document.querySelectorAll('.section').forEach(section => {
            observer.observe(section);
        });

        document.getElementById('deployLink').addEventListener('click', function(e) {
            e.preventDefault();
            alert('Replace with actual deployed website URL');
        });
    </script>

</body>
</html>
