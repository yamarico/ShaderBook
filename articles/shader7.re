= 応用編：SDFと数学的アプローチ

これまでの章で、シェーダーを用いたさまざまなグラフィックス表現を学んできました。この章では、さらに高度な数学的アプローチとして、SDF（Signed Distance Function）を用いた3D形状の生成について探求します。SDFは、物体からの距離情報を関数として表現し、それを利用して複雑な形状を簡潔に定義する手法です。

まずはSDFの基礎を理解し、その応用方法を学びます。そして、Unityのシェーダーを用いてSDFによる形状生成を実装し、リアルタイムで3Dオブジェクトを描画する方法を解説します。

== SDF（Signed Distance Function）の基礎

=== 距離関数とその応用

距離関数とは、空間内の任意の点から特定のオブジェクトまでの最短距離を返す関数です。符号付き距離関数（Signed Distance Function, SDF）は、さらにオブジェクトの内外を判定するために、距離に符号を持たせたものです。

- 正の値: オブジェクトの外側の点からの距離
- 負の値: オブジェクトの内側の点からの距離
- ゼロ: オブジェクトの表面上の点

=== SDFを使った形状生成

SDFを用いて基本的な形状を定義する方法を見ていきましょう。

==== 基本的なSDFの例

1. 球体のSDF
    \begin{align}
    sdSphere(p, r) = ||p|| - r
    \end{align}
    ここで、$p$ は空間内の点の位置ベクトル、$r$ は球の半径です。

2. ボックス（直方体）のSDF
    \begin{align}
    sdBox(p, b) = ||\max(\left|p\right| - b, 0)||
    \end{align}
    ここで、$b$ はボックスの半径ベクトル（各軸方向の半径）です。

3. トーラス（ドーナツ形状）のSDF
    \begin{align}
    sdTorus(p, t) = \sqrt{ ( \sqrt{ p_x^2 + p_z^2 } - t_x )^2 + p_y^2 } - t_y
    \end{align}
    ここで、$t$ はトーラスのパラメータ（大半径$t_x$、小半径$t_y$）です。

== UnityでSDFを用いた形状描画

それでは、SDFを用いてUnityのシェーダーで3D形状を描画してみましょう。ここでは、レイマーチングを使用してリアルタイムにSDFによる形状をレンダリングします。

=== レイマーチングの基本原理

レイマーチング（Ray Marching）は、レイトレーシングの一種で、レイを一定のステップで進めながらシーンとの衝突を検出する手法です。SDFを用いることで、レイがオブジェクトに到達するまでの距離を効率的に計算できます。

==== レイマーチングの手順

1. レイの発射: 各ピクセルから視線方向にレイを発射します。
2. 距離の計測: SDFを用いてレイ先の点から最も近いオブジェクトまでの距離を取得します。
3. ステップの進行: 取得した距離分だけレイを進めます。
4. ヒット判定: 距離が閾値以下になればオブジェクトにヒットしたと判定します。
5. 最大ステップ数の制御: 一定のステップ数を超えた場合は、背景として処理します。

=== SDFシェーダーの実装

==== シェーダーの作成

1. シェーダーファイルの作成: `Assets`フォルダで右クリックし、`Create > Shader > Unlit Shader`を選択します。シェーダー名を`RayMarchingSDFShader`とします。

==== シェーダーコードの編集

```hlsl
Shader "Custom/RayMarchingSDFShader"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
        _LightDir ("Light Direction", Vector) = (0,1,0,0)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            float4 _Color;
            float4 _LightDir;

            // SDF関数：球体
            float sdSphere(float3 p, float s)
            {
                return length(p) - s;
            }

            // 法線の計算
            float3 calcNormal(float3 p)
            {
                float eps = 0.001;
                float dx = sdScene(p + float3(eps, 0, 0)) - sdScene(p - float3(eps, 0, 0));
                float dy = sdScene(p + float3(0, eps, 0)) - sdScene(p - float3(0, eps, 0));
                float dz = sdScene(p + float3(0, 0, eps)) - sdScene(p - float3(0, 0, eps));
                return normalize(float3(dx, dy, dz));
            }

            // シーンのSDF
            float sdScene(float3 p)
            {
                float sphere = sdSphere(p, 1.0);
                return sphere;
            }

            fixed4 frag(v2f_img i) : SV_Target
            {
                float3 ro = float3(0, 0, -5); // カメラ位置
                float2 uv = (i.uv - 0.5) * 2.0;
                float3 rd = normalize(float3(uv, 1)); // レイ方向

                float totalDist = 0.0;
                int maxSteps = 100;
                float minDist = 0.001;
                float maxDist = 100.0;
                float t = 0.0;
                bool hit = false;

                for(int i = 0; i < maxSteps; i++)
                {
                    float3 p = ro + rd * t;
                    float dist = sdScene(p);
                    if(dist < minDist)
                    {
                        hit = true;
                        break;
                    }
                    if(t > maxDist)
                    {
                        break;
                    }
                    t += dist;
                }

                if(hit)
                {
                    float3 p = ro + rd * t;
                    float3 normal = calcNormal(p);
                    float diffuse = saturate(dot(normal, normalize(_LightDir.xyz)));
                    return fixed4(_Color.rgb * diffuse, 1);
                }
                else
                {
                    return fixed4(0, 0, 0, 1); // 背景色
                }
            }
            ENDCG
        }
    }
}
==== コードの解説

Properties:
_Color: オブジェクトの色を設定します。
_LightDir: 光源の方向を指定します。
sdSphere: 球体のSDF関数を定義します。
calcNormal: 微小な偏差を用いて、SDFの勾配（法線）を数値的に計算します。
sdScene: シーン内のオブジェクトを定義します。ここでは球体のみですが、他の形状やブーリアン演算を用いて複雑なシーンを構築できます。
frag関数: レイマーチングのメインループを実装します。各ピクセルに対してレイを発射し、オブジェクトとの衝突を判定します。ヒットした場合、法線を計算してライティングを行います。
==== マテリアルの作成と適用

マテリアルの作成: Assetsフォルダで右クリックし、Create > Materialを選択します。マテリアル名をRayMarchingSDFMaterialとします。
シェーダーの割り当て: マテリアルのインスペクターで、シェーダーをCustom/RayMarchingSDFShaderに設定します。
オブジェクトへの適用: カメラに向けたQuadをシーンに配置し、RayMarchingSDFMaterialを適用します。
==== 動作確認

シーンビューまたはゲームビューで、画面中央に球体が描画されていることを確認します。
マテリアルのColorやLight Directionを調整して、色やライティングを変更します。