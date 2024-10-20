= UnityでShaderを書こう！

前章までで、シェーダーの基礎知識やUnityでの開発環境の準備が整いました。この章では、実際にUnityを使ってシェーダーを作成し、さまざまなエフェクトを実装していきます。簡単なものから始めて、徐々に高度なテクニックに挑戦していきましょう。

== 基本のシェーダーを実装する

まずはシンプルなシェーダーを作成し、シェーダープログラミングの基本的な流れを理解しましょう。

=== 20行から始めるミニマルシェーダー

**目標**：最も基本的なシェーダーを作成し、オブジェクトに単一の色を表示します。

=== 手順

1. **Unityプロジェクトの作成**：
    - Unityを起動し、新しいプロジェクトを作成します。
    - テンプレートは「3D」を選択します。
2. **シェーダーファイルの作成**：
    - Projectウィンドウで、`Assets`フォルダ内で右クリックし、`Create > Shader > Unlit Shader`を選択します。
    - 新しいシェーダーに名前を付けます（例：`SimpleColorShader`）。
3. **シェーダーコードの編集**：
    - 作成したシェーダーファイルをダブルクリックして開きます。
    - 初期テンプレートが表示されますが、これを20行程度のシンプルなシェーダーに書き換えます。

=== コード解説

以下がミニマルなシェーダーコードです。

//listnum[hlsl][シェーダーコード]{
Shader "Custom/SimpleColorShader"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            float4 _Color;

            struct appdata
            {
                float4 vertex : POSITION;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                return _Color;
            }
            ENDCG
        }
    }
}
//}

=== コードのポイント

- **Shader宣言**：
    - `Shader "Custom/SimpleColorShader"`：シェーダーの名前を指定します。
    - `Custom/`プレフィックスを付けることで、Unityのシェーダーリストにカスタムカテゴリを作成できます。
- **Propertiesブロック**：
    - シェーダーに公開するパラメータを定義します。
    - `_Color`プロパティを定義し、インスペクターで色を変更できるようにします。
- **SubShaderブロック**：
    - 実際の描画処理を記述します。
    - `Pass`ブロック内でシェーダーコードを書きます。
- **CGPROGRAM**：
    - Cg/HLSL言語でシェーダーを記述することを宣言します。
    - `#pragma vertex vert`と`#pragma fragment frag`で、頂点シェーダーとフラグメントシェーダーのエントリーポイントを指定します。
- **構造体定義**：
    - `appdata`：頂点シェーダーの入力データ。
    - `v2f`：頂点シェーダーからフラグメントシェーダーへの出力データ。
- **頂点シェーダー`vert`**：
    - モデル空間の頂点座標をクリップ空間に変換します。
    - `UnityObjectToClipPos`関数を使用して座標変換を行います。
- **フラグメントシェーダー`frag`**：
    - 単純にカラーを返します。
    - `_Color`プロパティで指定した色をそのまま出力します。

=== シェーダーの適用

1. **マテリアルの作成**：
    - Projectウィンドウで右クリックし、`Create > Material`を選択します。
    - マテリアルに名前を付けます（例：`SimpleColorMaterial`）。
2. **シェーダーの割り当て**：
    - 作成したマテリアルを選択し、インスペクターで`Shader`をクリックします。
    - `Custom > SimpleColorShader`を選択します。
3. **オブジェクトへの適用**：
    - シーン内に適当な3Dオブジェクト（例：Cube）を配置します。
    - オブジェクトにマテリアルをドラッグ&ドロップします。
4. **動作確認**：
    - シーンビューやゲームビューで、オブジェクトが指定した色で表示されていることを確認します。
    - マテリアルの`Color`プロパティを変更して、色が変わることを確認します。

== インスペクターからのパラメータ設定

シェーダーにパラメータを追加し、インスペクターから動的に変更できるようにします。

=== 目標

- オブジェクトにテクスチャを適用し、テクスチャの色とカラーを乗算します。
- テクスチャやカラーをインスペクターから設定可能にします。

=== コードの追加

//listnum[hlsl][コード追加]{
Properties
{
    _MainTex ("Texture", 2D) = "white" {}
    _Color ("Color", Color) = (1,1,1,1)
}
//}

`_MainTex`プロパティを追加し、テクスチャを指定できるようにします。

=== フラグメントシェーダーの修正

//listnum[hlsl][フラグメントシェーダー修正]{
sampler2D _MainTex;
float4 _Color;

fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv;
    fixed4 texColor = tex2D(_MainTex, uv);
    return texColor * _Color;
}
//}

テクスチャのサンプリングを行い、カラーと乗算します。

=== v2f構造体の修正

//listnum[hlsl][v2f構造体修正]{
struct v2f
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
}
//}

UV座標をフラグメントシェーダーに渡すために、`uv`を追加します。

=== 頂点シェーダーの修正

//listnum[hlsl][頂点シェーダー修正]{
v2f vert (appdata v)
{
    v2f o;
    o.pos = UnityObjectToClipPos(v.vertex);
    o.uv = v.texcoord;
    return o;
}
//}

頂点からUV座標を受け取り、そのままフラグメントシェーダーに渡します。

=== シェーダーの適用と確認

1. **テクスチャの設定**：
    - マテリアルのインスペクターで、`Texture`プロパティにテクスチャを割り当てます。
    - 適当なテクスチャを`Assets`フォルダにインポートして使用します。
2. **動作確認**：
    - テクスチャがオブジェクトに表示され、`Color`プロパティで色調整ができることを確認します。

== 補間と疑似乱数の活用

シェーダー内で補間や疑似乱数を使用して、色や模様を動的に変化させます。

=== 目標

- オブジェクトにグラデーションを適用します。
- 時間経過に応じて色が変化する効果を実装します。

=== コードの追加

//listnum[hlsl][コード追加]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv;
    float gradient = uv.y;
    float time = _Time.y * 0.5;
    fixed4 color1 = fixed4(1, 0, 0, 1); // 赤
    fixed4 color2 = fixed4(0, 0, 1, 1); // 青
    fixed4 finalColor = lerp(color1, color2, abs(sin(time + gradient * 3.1415)));
    return finalColor;
}
//}

`uv.y`を使用して、垂直方向のグラデーションを計算します。`_Time.y`を使用して、時間経過に応じて色が変化するようにします。`lerp`関数で2つの色を補間し、`sin`関数で色の変化に緩急をつけます。

=== 動作確認

- シーンビューやゲームビューで、オブジェクトの色が時間とともに変化することを確認します。
- `color1`や`color2`を変更して、好みの色に調整します。

=== 応用：疑似乱数の生成

シェーダーで疑似乱数を生成し、模様やエフェクトに活用できます。

=== 疑似乱数関数の追加

//listnum[hlsl][疑似乱数関数の追加]{
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898,78.233))) * 43758.5453);
}
//}

疑似乱数を生成する関数`rand`を追加します。

=== フラグメントシェーダーの修正

//listnum[hlsl][フラグメントシェーダー修正]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 10.0; // スケールを調整
    float noise = rand(uv);
    fixed4 color = fixed4(noise, noise, noise, 1);
    return color;
}
//}

`rand`関数で生成したノイズを使用して、白黒のノイズパターンを表示します。`uv`を拡大して、ノイズの細かさを調整します。

=== 動作確認

- オブジェクトにランダムなノイズパターンが表示されることを確認します。
- `uv`のスケールや`rand`関数のパラメータを変更して、ノイズの見た目を調整します。

--- 

== ノイズの生成と応用

ノイズは自然な模様やエフェクトを作成するのに役立ちます。ここでは、ノイズの生成方法とその応用について学びます。

=== 値ノイズと勾配ノイズ

*値ノイズ（Value Noise）*と*勾配ノイズ（Gradient Noise）*は、シェーダーでよく使われるノイズの種類です。

=== 値ノイズの実装

値ノイズは、格子点にランダムな値を割り当て、その間を補間することで生成します。

=== コード例

//listnum[hlsl][値ノイズの実装]{
float valueNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);

    // 4つの格子点のランダム値を取得
    float a = rand(i);
    float b = rand(i + float2(1.0, 0.0));
    float c = rand(i + float2(0.0, 1.0));
    float d = rand(i + float2(1.0, 1.0));

    // 補間
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}
//}

格子点のランダム値を補間して、ノイズを生成します。

=== フラグメントシェーダーへの適用

//listnum[hlsl][フラグメントシェーダーへの適用]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 10.0;
    float noise = valueNoise(uv);
    fixed4 color = fixed4(noise, noise, noise, 1);
    return color;
}
//}

値ノイズを使用して、ブロック状のパターンを表示します。

=== 動作確認

- 値ノイズ特有のブロック状のノイズパターンが表示されることを確認します。
- `uv`のスケールを調整して、ノイズの細かさを変更します。

=== フラクタルノイズとワーリーノイズ

*フラクタルノイズ*は、異なる周波数と振幅のノイズを重ね合わせて生成します。

=== フラクタルノイズの実装

//listnum[hlsl][フラクタルノイズの実装]{
float fractalNoise(float2 uv)
{
    float total = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    for(int i = 0; i < 5; i++)
    {
        total += valueNoise(uv * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return total;
}
//}

異なる周波数と振幅のノイズを重ね合わせて、フラクタルノイズを生成します。

=== フラグメントシェーダーへの適用

//listnum[hlsl][フラクタルノイズ適用]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 5.0;
    float noise = fractalNoise(uv);
    fixed4 color = fixed4(noise, noise, noise, 1);
    return color;
}
//}

フラクタルノイズを使用して、滑らかな模様を表示します。

=== 動作確認

- フラクタルノイズによる滑らかな模様が表示されることを確認します。
- パラメータを調整して、模様の複雑さを変更します。

== ノイズの生成と応用

ノイズは自然な模様やエフェクトを作成するのに役立ちます。ここでは、ノイズの生成方法とその応用について学びます。

=== 値ノイズと勾配ノイズ

*値ノイズ（Value Noise）*と*勾配ノイズ（Gradient Noise）*は、シェーダーでよく使われるノイズの種類です。

=== 値ノイズの実装

値ノイズは、格子点にランダムな値を割り当て、その間を補間することで生成します。

==== コード例

//listnum[hlsl][値ノイズの実装]{
float valueNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);

    // 4つの格子点のランダム値を取得
    float a = rand(i);
    float b = rand(i + float2(1.0, 0.0));
    float c = rand(i + float2(0.0, 1.0));
    float d = rand(i + float2(1.0, 1.0));

    // 補間
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}
//}

格子点のランダム値を補間して、ノイズを生成します。

=== フラグメントシェーダーへの適用

//listnum[hlsl][フラグメントシェーダーへの適用]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 10.0;
    float noise = valueNoise(uv);
    fixed4 color = fixed4(noise, noise, noise, 1);
    return color;
}
//}

値ノイズを使用して、ブロック状のパターンを表示します。

=== 動作確認

- 値ノイズ特有のブロック状のノイズパターンが表示されることを確認します。
- `uv`のスケールを調整して、ノイズの細かさを変更します。

=== フラクタルノイズとワーリーノイズ

*フラクタルノイズ*は、異なる周波数と振幅のノイズを重ね合わせて生成します。

==== フラクタルノイズの実装

//listnum[hlsl][フラクタルノイズの実装]{
float fractalNoise(float2 uv)
{
    float total = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    for(int i = 0; i < 5; i++)
    {
        total += valueNoise(uv * frequency) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return total;
}
//}

異なる周波数と振幅のノイズを重ね合わせて、フラクタルノイズを生成します。

==== フラグメントシェーダーへの適用

//listnum[hlsl][フラクタルノイズ適用]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 5.0;
    float noise = fractalNoise(uv);
    fixed4 color = fixed4(noise, noise, noise, 1);
    return color;
}
//}

フラクタルノイズを使用して、滑らかな模様を表示します。

=== 動作確認

- フラクタルノイズによる滑らかな模様が表示されることを確認します。
- パラメータを調整して、模様の複雑さを変更します。

=== ワーリーノイズ（Cellular Noise）の紹介

ワーリーノイズは、細胞状の模様を生成するノイズです。実装が複雑なため、ここでは紹介のみにとどめます。

=== ノイズを使った質感表現

ノイズを用いて、大理石模様や雲のような質感を表現できます。

==== 大理石模様の生成

//listnum[hlsl][大理石模様の生成]{
fixed4 frag (v2f i) : SV_Target
{
    float2 uv = i.uv * 5.0;
    float noise = fractalNoise(uv);
    float marble = sin(uv.y * 10.0 + noise * 2.0);
    float colorValue = (marble + 1.0) * 0.5;
    fixed4 color = fixed4(colorValue, colorValue, colorValue, 1);
    return color;
}
//}

大理石のような縞模様を生成します。

==== 動作確認

- 大理石のような縞模様が表示されることを確認します。
- `uv`のスケールや`sin`関数のパラメータを調整して、模様を変化させます。

---

== サーフェスシェーダーのテクニック

サーフェスシェーダーを使って、より高度なエフェクトを実装します。

=== 透明・半透明シェーダーの作成

透明度を制御して、ガラスや水のような透明・半透明の表現を実現します。

==== シェーダーコード

//listnum[hlsl][透明・半透明シェーダーの作成]{
Shader "Custom/TransparentShader"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,0.5)
        _MainTex ("Texture", 2D) = "white" {}
    }
    SubShader
    {
        Tags { "Queue"="Transparent" "RenderType"="Transparent" }
        LOD 200

        Blend SrcAlpha OneMinusSrcAlpha // 透明度を有効にする

        CGPROGRAM
        #pragma surface surf Standard alpha

        sampler2D _MainTex;
        fixed4 _Color;

        struct Input
        {
            float2 uv_MainTex;
        };

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            fixed4 tex = tex2D(_MainTex, IN.uv_MainTex);
            o.Albedo = tex.rgb * _Color.rgb;
            o.Alpha = tex.a * _Color.a;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
//}

透明度を制御してガラスや水のような効果を実現するシェーダーです。

==== 適用と確認

- マテリアルを作成し、シェーダーを割り当てます。
- `Color`プロパティのアルファ値を調整して、透明度を変更します。
- シーン内で他のオブジェクトと重ね合わせて、透過具合を確認します。

=== リムライティングで雰囲気を演出

リムライティング（輪郭光）を追加して、オブジェクトの輪郭を強調します。

==== シェーダーコードの追加

//listnum[hlsl][リムライティングの追加]{
void surf (Input IN, inout SurfaceOutputStandard o)
{
    fixed4 tex = tex2D(_MainTex, IN.uv_MainTex);
    o.Albedo = tex.rgb * _Color.rgb;
    o.Alpha = tex.a * _Color.a;

    // リムライティングの計算
    float3 viewDir = normalize(_WorldSpaceCameraPos - mul(unity_ObjectToWorld, float4(IN.worldPos,1)).xyz);
    float rim = 1.0 - saturate(dot(normalize(o.Normal), viewDir));
    o.Emission = _Color.rgb * rim;
}
//}

ビュー方向の計算とリムライト強度の計算を行い、輪郭を強調するリムライティングを追加します。

==== 適用と確認

- オブジェクトの輪郭が明るくなり、幻想的な雰囲気が演出されていることを確認します。
- `Color`プロパティでリムライトの色を調整します。

=== テクスチャを用いた効果

テクスチャを活用して、特殊な効果を実現します。

==== ステンドグラスシェーダー

カラフルなステンドグラス風のシェーダーを作成します。

**手順**

1. **セルルックアップテクスチャの準備**：
    - カラフルなパレットを持つテクスチャを用意します。
2. **シェーダーコードの追加**：

//listnum[hlsl][ステンドグラスシェーダー]{
Properties
{
    _MainTex ("Texture", 2D) = "white" {}
    _CellTex ("Cell Texture", 2D) = "white" {}
}

void surf (Input IN, inout SurfaceOutputStandard o)
{
    fixed4 tex = tex2D(_MainTex, IN.uv_MainTex);
    fixed4 cell = tex2D(_CellTex, float2(tex.r, 0.0));
    o.Albedo = cell.rgb;
    o.Alpha = 1.0;
}
//}

テクスチャを使用して、カラフルなステンドグラス風の表現を実現します。

==== 適用と確認

- ステンドグラスのようなカラフルな模様が表示されることを確認します。

==== UVスクロールで水面を表現

UV座標を時間に応じて変化させ、水面の流れを表現します。

==== シェーダーコードの追加

//listnum[hlsl][UVスクロール]{
void surf (Input IN, inout SurfaceOutputStandard o)
{
    float2 uv = IN.uv_MainTex;
    uv += _Time.y * float2(0.1, 0.2); // 時間に応じてUVを移動
    fixed4 tex = tex2D(_MainTex, uv);
    o.Albedo = tex.rgb;
    o.Alpha = tex.a;
}
//}

UV座標を時間に応じてスクロールさせ、テクスチャが流れるように見せます。

==== 適用と確認

- オブジェクト上でテクスチャが流れるように見えることを確認します。
- `float2(0.1, 0.2)`の値を変更して、流れる速度や方向を調整します。

=== トゥーンシェーダーの自作

セルシェーディングを実装し、アニメ風の表現を行います。

==== シェーダーコード

//listnum[hlsl][トゥーンシェーダー]{
Shader "Custom/ToonShader"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
        #pragma surface surf ToonRamp

        fixed4 _Color;

        sampler2D _RampTex; // トーンマップ用のテクスチャ

        struct Input
        {
            float2 uv_MainTex;
            float3 viewDir;
        };

        void surf (Input IN, inout SurfaceOutput o)
        {
            fixed nl = dot(o.Normal, _WorldSpaceLightPos0.xyz);
            fixed diff = tex2D(_RampTex, float2(nl, 0)).r;
            o.Albedo = _Color.rgb;
            o.Alpha = _Color.a;
            o.Emission = _Color.rgb * diff;
        }
        ENDCG
    }
    FallBack "Diffuse"
}
//}

トーンマップテクスチャを用いて、アニメ風のトゥーンシェーディングを実現します。

==== 適用と確認

- オブジェクトがアニメ風のベタ塗りで表示されることを確認します。
- トーンマップテクスチャを調整して、影の段階を変更します。

---

== 動的なエフェクトの実装

時間や入力に応じて変化する動的なエフェクトを実装します。

=== 円やリングのアニメーション

中心から広がる円やリングのエフェクトを作成します。

==== シェーダーコード

//listnum[hlsl][リングエフェクト]{
Shader "Custom/RingEffect"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
        _Speed ("Speed", Float) = 1.0
    }
    SubShader
    {
        Tags { "RenderType"="Transparent" }
        Blend SrcAlpha OneMinusSrcAlpha

        CGPROGRAM
        #pragma surface surf Unlit alpha

        fixed4 _Color;
        float _Speed;

        struct Input
        {
            float2 uv;
        };

        void surf (Input IN, inout SurfaceOutput o)
        {
            float2 uv = IN.uv * 2.0 - 1.0;
            float len = length(uv);
            float time = frac(_Time.y * _Speed);
            float width = 0.1;

            float alpha = smoothstep(time, time - width, len);
            o.Emission = _Color.rgb * alpha;
            o.Alpha = alpha;
        }
        ENDCG
    }
}
//}

中心から広がるリングを表示するエフェクトです。

==== 適用と確認

- オブジェクト上でリングが拡大していくエフェクトが表示されることを確認します。
- `_Speed`パラメータを調整して、リングの拡大速度を変更します。

=== Dissolve（溶解）エフェクト

オブジェクトが溶けて消えていくようなエフェクトを実装します。

==== シェーダーコード

//listnum[hlsl][Dissolveエフェクト]{
Shader "Custom/DissolveEffect"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _NoiseTex ("Noise Texture", 2D) = "white" {}
        _DissolveAmount ("Dissolve Amount", Range(0,1)) = 0.0
        _Color ("Color", Color) = (1,1,1,1)
    }
    SubShader
    {
        Tags { "RenderType"="Transparent" }
        Blend SrcAlpha OneMinusSrcAlpha

        CGPROGRAM
        #pragma surface surf Standard alpha

        sampler2D _MainTex;
        sampler2D _NoiseTex;
        fixed4 _Color;
        float _DissolveAmount;

        struct Input
        {
            float2 uv_MainTex;
        };

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            fixed4 tex = tex2D(_MainTex, IN.uv_MainTex);
            float noise = tex2D(_NoiseTex, IN.uv_MainTex * 5.0).r;
            float alpha = smoothstep(_DissolveAmount - 0.1, _DissolveAmount + 0.1, noise);
            o.Albedo = tex.rgb * _Color.rgb;
            o.Alpha = alpha * tex.a * _Color.a;
        }
        ENDCG
    }
}
//}

ノイズテクスチャを使用して、溶解エフェクトを実装します。

==== 適用と確認

- マテリアルの`Dissolve Amount`を徐々に増やして、オブジェクトが溶けて消える様子を確認します。
- ノイズテクスチャを変更して、溶解パターンを変化させます。

=== 雪を降らせるシェーダー

シェーダーで雪が降るエフェクトを実装します。

==== シェーダーコード

//listnum[hlsl][雪エフェクト]{
Shader "Custom/SnowfallEffect"
{
    Properties
    {
        _SnowTex ("Snow Texture", 2D) = "white" {}
        _Speed ("Speed", Float) = 1.0
        _Density ("Density", Float) = 1.0
    }
    SubShader
    {
        Tags { "Queue"="Transparent" "RenderType"="Transparent" }
        Blend SrcAlpha OneMinusSrcAlpha

        CGPROGRAM
        #pragma surface surf Unlit alpha

        sampler2D _SnowTex;
        float _Speed;
        float _Density;

        struct Input
        {
            float2 uv;
        };

        void surf (Input IN, inout SurfaceOutput o)
        {
            float time = _Time.y * _Speed;
            float2 uv = IN.uv;
            uv.y += time;

            float noise = tex2D(_SnowTex, uv * _Density).r;
            float alpha = smoothstep(0.5, 0.6, noise);

            o.Emission = fixed3(1,1,1) * alpha;
            o.Alpha = alpha;
        }
        ENDCG
    }
}
//}

ノイズを使って雪が降るエフェクトを実現します。

==== 適用と確認

- 平面やQuadにマテリアルを適用し、シーン全体に雪が降るように配置します。
- `_Speed`や`_Density`を調整して、雪の降り方を変更します。
