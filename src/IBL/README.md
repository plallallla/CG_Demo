# 从渲染方程到pbr

$$
L_o(\mathbf{x}, \omega_o) = L_e(\mathbf{x}, \omega_o) + \int_{\Omega} f_r(\mathbf{x}, \omega_i, \omega_o) \, L_i(\mathbf{x}, \omega_i) \, (\mathbf{n} \cdot \omega_i) \, d\omega_i
$$

$$
L_o \approx
\underbrace{
\sum_{j} \left[
(1 - F_j)(1 - m) \frac{\text{albedo}}{\pi} + \frac{D_j G_j F_j}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\mathbf{l}_j)}
\right] L_j (\mathbf{n} \cdot \mathbf{l}_j)
}_{\text{Direct Lighting}}\\
+\\
\underbrace{
(1 - F_{\text{env}})(1 - m) \cdot \text{Irradiance}(\mathbf{n}) \cdot \text{albedo}
+ 
\text{Prefilter}(\mathbf{r}, \text{rough}) \cdot \left( F_{\text{env}} \cdot \text{BRDF}_x + \text{BRDF}_y \right)
}_{\text{Indirect Lighting (IBL)}}
\cdot \text{AO}
$$



## 一、原始渲染方程

$$
L_o(\mathbf{x}, \omega_o) = L_e(\mathbf{x}, \omega_o) + \int_{\Omega} f_r(\mathbf{x}, \omega_i, \omega_o) , L_i(\mathbf{x}, \omega_i) , (\mathbf{n} \cdot \omega_i) , d\omega_i
$$

- 假设无自发光（(L_e = 0)），这是大多数非光源物体的情况。
- 目标：高效计算该积分。

## 二、将入射光 (L_i) 拆分为两部分

我们将环境中的入射辐射亮度 (L_i(\omega_i)) 分解为：

1. **来自有限个解析光源的直接光照**（如点光源、平行光）  
2. **来自环境的连续间接光照**（如天空、周围物体反射）

即： 
$$
L_i(\omega_i) = \underbrace{\sum_{j=1}^{N} L_j , \delta(\omega_i - \omega_j)}*{\text{Direct}} + \underbrace{L*{\text{env}}(\omega_i)}_{\text{Indirect (IBL)}}
$$


其中 $\delta$ 是狄拉克函数，表示方向上的点光源。

代入原方程得：

$$
L_o = 
\underbrace{
\sum_j f_r(\omega_j, \omega_o) \, L_j (\mathbf{n} \cdot \omega_j)
}_{\text{Direct Lighting}}
+
\underbrace{
\int_{\Omega} f_r(\omega_i, \omega_o) \, L_{\text{env}}(\omega_i) \, (\mathbf{n} \cdot \omega_i) \, d\omega_i
}_{\text{Indirect Lighting (IBL)}}
$$


------

## 三、采用 Cook-Torrance BRDF 模型

PBR 使用 **Cook-Torrance 微表面 BRDF**，其形式为：
$$
f_r = f_r^{\text{diffuse}} + f_r^{\text{specular}} = k_D \cdot \frac{\text{albedo}}{\pi} + k_S \cdot \frac{D G F}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\mathbf{l})} 
$$
其中：

- $k_S = F$（镜面反射比例 = Fresnel 项）
- $k_D = (1 - F)(1 - \text{metallic})$（漫反射比例，金属无漫反射）
- $D$：法线分布函数（GGX）
- $G$：几何遮蔽项（Smith）
- $F$：菲涅尔反射率

## 四、直接光照的离散近似

将 BRDF 代入直接光求和项：
$$
L_o^{\text{direct}} = \sum_j \left[  (1 - F_j)(1 - m) \frac{\text{albedo}}{\pi} 

+  \frac{D_j G_j F_j}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\mathbf{l}_j)} \right] L_j (\mathbf{n} \cdot \mathbf{l}_j)
$$
这正是着色器中 `for` 循环计算的部分。

## 五、间接光照的 Split-Sum 近似（IBL）

间接项为：

$$ L_o^{\text{indirect}} = \int_{\Omega} \left( f_r^{\text{diffuse}} + f_r^{\text{specular}} \right) L_{\text{env}}(\omega_i) (\mathbf{n} \cdot \omega_i) , d\omega_i $$

由于无法实时积分，采用 **Split-Sum Approximation**（由 Epic Games 提出）：

### 1. 漫反射部分（低频，与视角无关）

$$ \int f_r^{\text{diffuse}} L_{\text{env}}(\omega_i) (\mathbf{n} \cdot \omega_i) d\omega_i  \approx  (1 - F_{\text{env}})(1 - m) \cdot \text{albedo} \cdot \underbrace{\int L_{\text{env}}(\omega_i) (\mathbf{n} \cdot \omega_i) d\omega_i}_{\text{IrradianceMap}(\mathbf{n})} $$

- 预计算为 **irradiance cube map**。

### 2. 镜面反射部分（高频，与视角和粗糙度相关）

理想形式： $$ \int \frac{D G F}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\omega_i)} L_{\text{env}}(\omega_i) (\mathbf{n} \cdot \omega_i) d\omega_i $$

Split-Sum 近似将其**分离为两个独立预计算项**：

$$ \approx  \underbrace{\int L_{\text{env}}(\omega_i) D(\omega_h) (\mathbf{n} \cdot \omega_i) d\omega_i}*{\text{PrefilterMap}(\mathbf{r}, \text{rough})} \cdot \underbrace{\int \frac{G F}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\omega_i)} (\mathbf{n} \cdot \omega_i) d\omega_i}*{\text{BRDF LUT}( \mathbf{n}\cdot\mathbf{v}, \text{rough} )} $$

- 第一项：对环境贴图按不同粗糙度进行**重要性采样卷积**，存为 **prefiltered environment map**
- 第二项：将 $G$ 和 $F$的积分结果（依赖于 ($n\cdot v$) 和$roughness$）预计算为 **2D BRDF 查找表（LUT）**

最终镜面 IBL 表达为：

$$ \text{Specular}*{\text{IBL}} = \text{Prefilter}(\mathbf{r}, \text{rough}) \cdot \left( F*{\text{env}} \cdot \text{BRDF}_x + \text{BRDF}_y \right) $$

其中：

- $F_{\text{env}} = F_0 + (1 - F_0)(1 - \cos\theta)^5$，且考虑粗糙度修正（使用 `fresnelSchlickRoughness`）
- $\text{BRDF}_x, \text{BRDF}_y$ 来自 LUT 的两个通道（分别对应 Fresnel 缩放和偏移）

### 3. 合并漫反射与镜面反射，并乘以 AO

$$ L_o^{\text{indirect}} = \Big[ (1 - F_{\text{env}})(1 - m) \cdot \text{Irradiance}(\mathbf{n}) \cdot \text{albedo} + \text{Specular}_{\text{IBL}} \Big] \cdot \text{AO} $$

> 注意：AO（环境光遮蔽）是**非物理的经验项**，用于模拟小尺度几何遮挡，在真实渲染方程中不存在，但实践中广泛使用。

## 六、最终近似公式

综上，原始渲染方程被近似为：
$$
L_o \approx
\underbrace{
\sum_{j} \left[
(1 - F_j)(1 - m) \frac{\text{albedo}}{\pi} + \frac{D_j G_j F_j}{4 (\mathbf{n}\cdot\mathbf{v})(\mathbf{n}\cdot\mathbf{l}_j)}
\right] L_j (\mathbf{n} \cdot \mathbf{l}_j)
}_{\text{Direct Lighting}}\\
+\\
\underbrace{
(1 - F_{\text{env}})(1 - m) \cdot \text{Irradiance}(\mathbf{n}) \cdot \text{albedo}
+ 
\text{Prefilter}(\mathbf{r}, \text{rough}) \cdot \left( F_{\text{env}} \cdot \text{BRDF}_x + \text{BRDF}_y \right)
}_{\text{Indirect Lighting (IBL)}}
\cdot \text{AO}
$$

## 七、总结：近似策略

| 步骤            | 方法                                          | 目的                   |
| --------------- | --------------------------------------------- | ---------------------- |
| 1. 光源分解     | (L_i = \text{点光源} + L_{\text{env}})        | 分离直接/间接光        |
| 2. BRDF 拆分    | (f_r = f_r^{\text{diff}} + f_r^{\text{spec}}) | 支持金属/非金属材质    |
| 3. 直接光离散化 | 用求和代替积分                                | 实时计算有限光源       |
| 4. IBL 预计算   | Irradiance Map + Prefilter Map + BRDF LUT     | 将复杂积分转为纹理采样 |
| 5. Split-Sum    | 分离镜面积分中的 (L_{\text{env}}) 与 (G,F)    | 使预计算可行           |
| 6. 添加 AO      | 经验乘子                                      | 增强视觉真实感         |
