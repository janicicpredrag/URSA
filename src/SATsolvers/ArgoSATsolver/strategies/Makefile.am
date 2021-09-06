# Copyright (C) 2007-2009. Filip Maric, Predrag Janicic
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# This program is inspired by MiniSat solver (C) Een, Sorensson 2003-2006.

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/auxiliary -I$(top_srcdir)/src/basic-types

noinst_LTLIBRARIES = libstrategies.la

noinst_HEADERS =					\
	Activities.hpp					\
	ActivityMeasure.hpp				\
	RestartStrategy.hpp				\
	RestartStrategyNoRestart.hpp			\
	RestartStrategyConflictCounting.hpp		\
	RestartStrategyMinisat.hpp			\
	RestartStrategyPicosat.hpp			\
	RestartStrategyLuby.hpp				\
	RestartStrategyFunction.hpp			\
	ForgetStrategy.hpp				\
	ForgetStrategyNoForget.hpp			\
	ForgetStrategyMinisat.hpp			\
	ForgetStrategyFunction.hpp			\
	ForgetSelectionStrategy.hpp			\
	ForgetSelectionStrategyMinisat.hpp		\
	ForgetSelectionStrategyClauseLength.hpp		\
	LiteralSelectionStrategy.hpp			\
	LiteralSelectionStrategyVariablePolarity.hpp	\
	VariableSelectionStrategy.hpp			\
	VariableSelectionStrategyFirstUndefined.hpp	\
	VariableSelectionStrategyMinisat.hpp		\
	VariableSelectionStrategyRandomDecorator.hpp	\
	LiteralPolaritySelectionStrategy.hpp

libstrategies_la_SOURCES =				\
	RestartStrategy.cpp				\
	RestartStrategyConflictCounting.cpp		\
	RestartStrategyMinisat.cpp			\
	RestartStrategyPicosat.cpp			\
	RestartStrategyLuby.cpp				\
	RestartStrategyFunction.cpp			\
	ForgetStrategy.cpp				\
	ForgetStrategyMinisat.cpp			\
	ForgetStrategyFunction.cpp			\
	ForgetSelectionStrategy.cpp			\
	ForgetSelectionStrategyMinisat.cpp		\
	ForgetSelectionStrategyClauseLength.cpp		\
	LiteralSelectionStrategy.cpp			\
	VariableSelectionStrategy.cpp			\
	VariableSelectionStrategyFirstUndefined.cpp	\
	VariableSelectionStrategyMinisat.cpp		\
	VariableSelectionStrategyRandomDecorator.cpp	\
	LiteralPolaritySelectionStrategy.cpp

libstrategies_la_CXXFLAGS = -ffloat-store
