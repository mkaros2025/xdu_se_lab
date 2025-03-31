# 程序设计综合实训的前端部分

## 项目介绍
为什么只有前端部分呢，因为后端不是我写的
两天时间极限敏捷开发的成果
所以写的非常烂，eslint 会报很多警告，（于是我把 eslint 干掉了

极简实现，甚至没有 jwt，（没错你可以直接通过 curl 改数据库

最好笑的是虽然提供了评价页面，但是没有提供查看评价页面，于是评价的内容只能在数据库中查看
如果有人愿意完善的话欢迎 pr

## 功能特点
- 用户认证（登录/注册）
- 浏览菜单和点餐
- 用户订单管理
- 餐厅工作人员菜单管理
- 餐厅工作人员订单处理
- 配送人员配送管理
- 用户评价系统

## 技术栈
- Vue 3
- Vuex 4（状态管理）
- Vue Router 4（路由管理）
- Vuetify 3（UI组件库）
- Axios（HTTP客户端）
- Material Design Icons

## 项目结构
```
frontend/
├── public/          # 静态资源
├── src/             # 源代码
│   ├── assets/      # 图片、样式等资源
│   ├── components/  # 可复用的组件
│   ├── plugins/     # Vue插件配置
│   ├── router/      # 路由配置
│   ├── services/    # API服务
│   ├── store/       # Vuex状态管理
│   ├── views/       # 页面组件
│   ├── App.vue      # 根组件
│   └── main.js      # 入口文件
└── package.json     # 项目依赖
```

## 用户角色
- **普通用户**: 浏览菜单、下单、查看订单历史、评价
- **餐厅工作人员**: 管理菜单、处理订单
- **配送人员**: 管理配送任务

## 安装与运行
### 安装依赖
```
cd frontend
npm install
```

### 开发环境运行
```
npm run serve
```

### 生产环境构建
```
npm run build
```

## 路由说明
- `/`: 首页
- `/login`: 登录页面
- `/register`: 注册页面
- `/menu`: 菜单页面（需登录）
- `/orders`: 用户订单列表（需登录）
- `/orders/:id`: 订单详情页（需登录）
- `/reviews`: 评价页面（需登录）
- `/manage-menu`: 菜单管理（仅工作人员）
- `/manage-orders`: 订单管理（仅工作人员）
- `/deliveries`: 配送管理（仅配送人员）