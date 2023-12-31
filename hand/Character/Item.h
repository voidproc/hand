﻿#pragma once

namespace hand
{
	enum class ItemType
	{
		Money,
	};

	int ItemScore(ItemType type);

	class Item;
	using ItemPtr = std::unique_ptr<Item>;

	class Item
	{
	public:
		Item(ItemType type, Effect& effect, const Vec2& pos);

		ItemType type() const;

		virtual void update();

		virtual void draw() const;

		virtual RectF collision() const;

		bool isAlive() const;

		void kill();

		// falseの場合衝突判定をしないでください
		virtual bool enableCollision() const;

		const Vec2& pos() const;

	private:
		ItemType type_;

	protected:
		Effect& effect_;

		Vec2 pos_;

		int life_;

		Stopwatch time_;
	};

	class ItemMoney : public Item
	{
	public:
		ItemMoney(ItemType type, Effect& effect, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

		bool enableCollision() const override;

	private:
		Vec2 vel_;
		double animSpeed_;
	};

	template <class T, ItemType I, class ... Args>
	ItemPtr MakeItem(Args&& ... args)
	{
		return std::make_unique<T>(I, std::forward<Args>(args)...);
	}
}
